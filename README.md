# QTicketBai

QTicketBai is an attempt at implementing the TicketBai and LROE protocols using Qt.

## Building

The library depends on the following libraries:

* [QtCurl](https://github.com/planed-es/QtCurl)
* [QXmlSec](https://github.com/planed-es/QXmlSec)
* QtCore
* QtNetwork
* QtXml
* QtGui (optional, needed for QR code generation)
* zlib

Make sure these are all installed on your system. Afterwards, run the following commands
from the root of this repository:

```
mkdir build
cd build
cmake ..
make
```

If you're planning on using QTicketBai from a server, then you might not want to bring
QtGui into the equation. You can disable the dependency to QtGui by replacing `cmake ..`
with `cmake .. -DWITH_GUI=OFF`. Note that this will disable the feature providing QRCode
generation for the TicketBAI invoices.

## Configuration

You can configure QTicketBai by creating a QTicketBai object:

```
QTicketBai ticketBai;

ticketBai.certificate()
  .withPath("/path/to/your/p12/certificate")
  .withPassword("Your certificate's password");
  .prepare();
ticketBai.software()
  .withLicense("LICENSENUMBER")
  .withCif("Software Developer CIF number")
  .withName("Software Developer name");
ticketBai
  .withTaxAuthorityUrl(QUrl("https://batuz.eus/QRTBAI/"))
  .withDumpPath("/path/to/storage/folder/for/signed/documents")
  .withEmitter(CompanyData{
    "My company name",            // name
    "15 camino del caminante",    // address
    "Bilbao",                     // city
    "12345",                      // postal code
    TbaiContactDefines::NifIvaId, // id type
    "A99805061"                   // id number
  })
  .withDeveloper(CompanyData{
    "QTicketBai Developers",
    "16 camino del caminante",
    "Northrend",
    "54321",
    TbaiContactDefines::NifIvaId,
    "Software Developer CIF number",
  });
```

The QTicketBai object works as a singleton: there can only be one instance of QTicketBai at any given time
in your application. By default, all the features of the ticketbai-qt library will use these settings
when signin documents or interacting with the TicketBAI service.

## Usage
### Initializing
#### Setting up user and developer informations
The first thing you want to do is to setup the user information for the entity
that will use your program to manage their invoices, as well as your own
information as a TicketBAI developer entity:

```
#include <ticketbai-qt/qticketbai.h>

const CompanyData user = {
  "PlanED S.L.",
  "1 Camino del Caminante",
  "City",
  "Postal code",
  TbaiContactDefines::NifIvaId, // ID type, as defined in ticketbai-qt/tbaicontactdefines.h
  "ID", // Cif, nif, depending on the ID type
  "Phone number",
  "Fax number",
  "Email"
};

int main()
{
  QTicketBai ticketbai; // QTicketBai instance

  ticketbai.withUser(user);
  ticketbai.withDeveloper(user);
  reutrn 0;
}
```

#### Check your settings
The second thing you want to do is to check that your settings are correct. You can use
the `TbaiSignProcess::checkSettings` function to check the validity of your settings:

```
#include <ticketbai-qt/qticketbai.h>
#include <ticketbai-qt/tbaisignprocess.h>

const CompanyData user = ...

int main()
{
  QTicketBai ticketbai;

  ticketbai.withUser(user)
  ticketbai.withDeveloper(user);
  ticketBai.certificate()
    .withPath("/path/to/your/p12/certificate")
    .withPassword("Your certificate's password");
    .prepare(); // Do not forget to call `prepare` on your TbaiCertificate once you've set its settings
  if (TbaiSignProcess::checkSettings())
  {
    return 0; // QTicketBai is ready to sign and submit invoices
  }
  return -1; // Something's missing in your environment
}
```

The `TbaiSignProcess::checkSettings` function will also display the diagnostic results to
[stderr](https://cplusplus.com/reference/cstdio/stderr/), allowing you to see what's
missing from your environment to get this thing up and running.

#### Initializing QXmlSec
QXmlSec is a dependency of QTicketBai, and it needs to be initialized before any
document signing is performed. Just make sure you maintain an instance of the
`QXmlSec` object while such operations are running:

```
#include <ticketbai-qt/qticketbai.h>
#include <ticketbai-qt/tbaisignprocess.h>
#include <xmlsec-qt/xmlsec.h> // QXmlSec header

const CompanyData user = ...

int main()
{
  QXmlSec xmlsec; // QXmlSec instance
  QTicketBai ticketbai;

  ticketbai.withUser(user)
  ticketbai.withDeveloper(user);
  if (TbaiSignProcess::checkSettings())
  {
    return 0;
  }
  return -1;
}
```

### Submitting invoices
#### Implementing the invoice interface
The next step is to create your own invoices objects, implementing the `TbaiInvoiceInterface`
defined in `ticketbai-qt/tbaiinvoiceinterface.h`:

```
#include <ticketbai-qt/tbaiinvoiceinterface.h>
class MyInvoice : public TbaiInvoiceInterface
{
public:
  // previousInvoice shall return the previous invoice in the TicketBAI
  // signing chain, or nullptr if this is the first invoice to get signed.
  TbaiInvoiceInterface* previousInvoice() const override;

  // Returns a value from the TbaiInvoiceInterface::Type enum,
  // corresponding to the invoice rectificative type (or just
  // InvoiceType if the invoice isn't rectificative) defined
  // in the TicketBAI specification with the key L7.
  Type invoiceType() const override;

  // Returns the TicketBAI signature for this invoice, or a null QByteArray
  // if the invoice hasn't been signed yet.
  const QByteArray& signature() const override;

  // Returns the date and time of the invoice emission.
  const QDateTime& date() const override;

  // Returns the `series` value for this invoice. Optional
  const QByteArray& series() const override;

  // Returns the invoice number
  const QByteArray& number() const override;

  // Returns a name for the invoice
  const QString& name() const override;

  // Returns a description for the invoice
  const QString& description() const override;

  // Returns the list of recipients for this invoice, represented as
  // CompanyData (see ticketbai-qt/companydata.h)
  const Recipients& recipients() const override;

  // Returns a list of the invoice amounts for each included VATs policies
  QList<TbaiInvoiceInterface::VatBreakdown> vatBreakdowns() const override;
};
```

The convenience `TbaiInvoiceComponent` class is also available in `ticketbai-qt/invoicecomponent.h`,
providing a simple QObject-based implementation for `TbaiInvoiceInterface`.

##### Invoice amounts using VatBreakdown

The `TbaiInvoiceInterface::VatBreakdown` type allows you to define invoice amounts for each VAT
policy. Any couple VAT policy and VAT/Recargo rate can only be expressed up to one time in the resulting list.
Let's see a simple example:

```
TbaiInvoiceInterface::VatBreakdown vatBreakdown;

vatBreakdown.base = 42.42; // tax base
vatBreakdown.taxRate = 0.2; // VAT rate at 20%
vatBreakdown.recargoRate = 0.014; // Recargo rate at 1.4%
```

For VAT exempted subjets, we would change that code as following:

```
TbaiInvoiceInterface::VatBreakdown vatBreakdown;

vatBreakdown.base = 42.42; // tax base
vatBreakdown.exemptionType = TbaiInvoiceInterface::VatExemptedByNormaForalArticle20;
```

And for subjects that are not subjects to VAT:

```
TbaiInvoiceInterface::VatBreakdown vatBreakdown;

vatBreakdown.base = 42.42;
vatBreakdown.vatState = TbaiInvoiceInterface::NotSubjectToVat;
```

#### Generating a TicketBAI signature for an invoice
Now that we can expose our custom invoices types to QTicketBai, the next thing we'll
want to do is to generate TicketBAI documents:

```
#include <ticketbai-qt/tbaisignprocess.h>
#include <iostream>

void sign_invoice(const TbaiInvoiceInterface& invoice)
{
  TbaiDocument document = TbaiDocument().createFrom(invoice);

  if (TbaiSignProcess::sign(document))
  {
    std::cout << "Generated signature: " << document.signature().constData() << std::endl;
    std::cout << "Generated xml:\n" << document.toString(2) << std::endl;
  }
  else
    std::cerr << "Failed to sign invoice" << std::endl;
}
```

Don't forget to store the return value of `document.signature()` in your invoice object,
so that it can be used later when signing your next invoice. To put it simply, for the next
invoice you will sign, you want `invoice.previousInvoice()->signature()` to return
the current value of `document.signature()`.

##### Catching errors
Errors might happen during the signing process. The `TbaiSignProcess::sign` function
return value can be used to get details about failures:

```
void sign_invoice(const TbaiInvoiceInterface& invoice)
{
  TbaiDocument document = TbaiDocument().createFrom(invoice);
  TbaiSignProcess::ReturnValue result;

  result = TbaiSignProcess::sign(document);
  if (result)
  {
    std::cout << "Generated signature: " << document.signature().constData() << std::endl;
    std::cout << "Generated xml:\n" << result.xml.constData() << std::endl;
  }
  else
    std::cerr << "Failed to sign invoice" << result.error.toStdString() << std::endl;
}
```

#### Loading a TicketBAI document from file

At anytime, you can load a TicketBAI document from a file:

```
#include <ticketbai-qt/tbaidocument.h>
#include <iostream>

int main(int argc, char** argv)
{
  TbaiDocument document;

  if (argc == 2)
  {
    if (document.loadFromFile(argv[1]))
    {
      std::cout << document.signature().constData() << std::endl;
      return 0;
    }
    else
      std::cerr << "invalid TicketBAI document " << argv[1] << std::endl;
  }
  else
    std::cerr << "usage: ./command [filepath]" << std::endl;
  return -1;
}
```

In case you intend to store your TicketBAI invoices another way, you
may also initialize a `TbaiDocument` directly from the XML source:

```
TbaiDocument document;
QFile file("/var/lib/ticketbai/invoice.xml");

if (file.open(QIODevice::ReadOnly))
{
  document.loadFrom(file.readAll());
  std::cout << document.signature().constData() << std::endl;
}
```

### Sending LROE documents

TODO: the LROESubmitProcess isn't completely implemented yet

```
#include <ticketbai-qt/lroesubmitprocess.h>

static void invoice_upload()
{
  LROESubmitProcess lroe;

  lroe.submitAll();
}

```

## Running the tests

The tests use the example and certificate sets provided by in the
[technical documentation](https://www.batuz.eus/es/documentacion-tecnica) for
the LROE process.

The `test.sh` script is provided to download the test environment and run all the tests.
It is designed to run from the root of the repository:

```
./test.sh
```
