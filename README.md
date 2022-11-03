# QTicketBai

QTicketBai is an attempt at implementing the TicketBai and LROE protocols using Qt.

## Building

The library depends on the following libraries:

* [QtCurl](https://github.com/planed-es/QtCurl)
* [QXmlSec](https://github.com/planed-es/QXmlSec)
* QtCore
* QtNetwork
* QtXml
* zlib

Make sure these are all installed on your system. Afterwards, run the following commands
from the root of this repository:

```
mkdir build
cd build
cmake ..
make
```

## Configuration

You can configure QTicketBai before runtime, by using environment variables:

```
LROE_ENVIRONMENT          # set to "production" when you want to stop using LROE's test environment
TBAI_OUTPUT_PATH          # directory in which your TicketBai invoices will be stored
TBAI_CERTIFICATE_PATH     # path to your pkcs12 certificate file
TBAI_CERTIFICATE_PASSWORD # password to yuor pkcs12 certificate
TBAI_LICENSE              # TODO
TBAI_TAX_AUTHORITY_URL    # Tax authority (ex: https://batuz.eus/QRTBAI)
```

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
  NifIvaId, // ID type, as defined in ticketbai-qt/companydata.h in the TbaiIdentityType enum
  "ID", // Cif, nif, depending on the ID type
  "Phone number",
  "Fax number",
  "Postal code",
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

#### Initializing QTicketBai
In the case of QTicketBai, some initialization is required before you make
any requests to the relevant LROE service. Namely, you need to pre-load your
certificate, which is to be done using the `TbaiCertificate` class:

```
#include <ticketbai-qt/qticketbai.h>
#include <ticketbai-qt/tbaisignprocess.h>
#include <ticketbai-qt/tbaicertificate.h>
#include <xmlsec-qt/xmlsec.h>

const CompanyData user = ...

int main()
{
  QXmlSec xmlsec;
  QTicketBai ticketbai;

  ticketbai.withUser(user)
  ticketbai.withDeveloper(user);
  if (TbaiSignProcess::checkSettings())
  {
    TbaiCertificate::prepare(); // loads your PKCS12 certificate
    return 0;
  }
  return -1;
}
```

### Submitting invoices
#### Implementing the invoice interface
The next step is to create your own invoices object, implementing the `TbaiInvoiceInterface`
defined in `ticketbai-qt/invoiceinterface.h`:

```
#include <ticketbai-qt/invoiceinterface.h>
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
};
```

The convenience `TbaiInvoiceComponent` class is also available in `ticketbai-qt/invoicecomponent.h`,
providing a simple QObject-based implementation for `TbaiInvoiceInterface`.

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
