# QTicketBai

QTicketBai is an attempt at implementing the TicketBai and LROE protocols using Qt.

## Building

The library depends on the following libraries:

* [QtCurl](https://github.com/Plaristote/QtCurl)
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
TBAI_SOFTWARE_CIF         # CIF of the company providing the TicketBai software
TBAI_SOFTWARE_NAME        # Name of the company providing the TicketBai software
TBAI_TAX_AUTHORITY_URL    # Tax authority (ex: https://batuz.eus/QRTBAI)
AUTOFIRMA_PATH            # directory in which QTicketBai expects to find `autofirma.jar`
```

### Company data

To define the identity of the emitter entity, you must define the `CompanyData::self` global
variable:

```
#include <ticketbai-qt/companydata.h>

const CompanyData CompanyData::self = {
  "PlanED S.L.",
  "1 Camino del Caminante",
  "City",
  "CIF number",
  "Phone number",
  "Fax number",
  "Postal code",
  "Email"
};
```

### AutoFirma

[AutoFirma](https://firmaelectronica.gob.es/Home/Descargas.html) is a software provided by the spanish
government to sign files using a pkcs12  certificate. QTicketBai uses AutoFirma to sign TicketBai invoices.

The default AutoFirma package contains much more than what's actually needed, which is why we provide
a script to extract the only one file actually needed by QTicektBai:

```
scripts/install-autofirma.sh "/usr/local/bin/autofirma.jar"
```

Note that QTicketBai will look for autofirma.jar using the `PATH` environment variable. Alternatively,
You can also set the `AUTOFIRMA_PATH` environment variable:

```
export AUTOFIRMA_PATH="/usr/local/bin"
```

## Usage
### Check the settings
The first thing you want to do is to check that your settings are correct. You can use
the `TbaiSignProcess::checkSettings` function to check the validity of your settings.
This function will check for your TicketBai settings, display the results to `stderr`,
and will return `true` or `false` depending on whether your settings seem valid or not:

```
#include <ticketbai-qt/tbaisignprocess.h>

int main()
{
  if (TbaiSignProcess::checkSettings())
  {
    return 0; // QTicketBai is ready to sign and submit invoices
  }
  return -1; // Something's missing in your environment
}
```

### Implementing invoices
The next step is to create your own invoices object, implementing the `TbaiInvoiceInterface`.

### Generating a TicketBAI signature for an invoice
The signing process is asynchronous, so you'll need to listen to signals in order to intercept
the generated signature and/or your definitive invoice XML including the signature.

##### Synchronous
Let's first see how to sign an invoice synchronously:

```
#include <ticketbai-qt/tbaisignprocess.h>
#include <iostream>

static void sign_invoice(const TbaiInvoiceInterface& invoice)
{
  TbaiSignProcess signProvider;

  QObject::connect(&signProvider, &TbaiSignProcess::generatedXml, [](QByteArray xml)
  {
    std::cout << "Generated xml:\n" << xml.toStdString() << std::endl;
  });
  signProvider.sign(invoice);
  signProvider.wait();
}
```

##### Asynchronous
In order to sign your invoices asynchronously, we'll just need to make use a pointer to
ensure our `TbaiSignProcess` object doesn't expire at the end of the scope. We'll then
listen to the `finished` signal to know when to clean up the pointer:

```
#include <ticketbai-qt/tbaisignprocess.h>
#include <iostream>

static void sign_invoice(const TbaiInvoiceInterface& invoice)
{
  TbaiSignProcess* signProvider = new TbaiSignProcess();

  QObject::connect(signProvider, &TbaiSignProcess::generatedXml, [](QByteArray xml)
  {
    std::cout << "Generated xml:\n" << xml.toStdString() << std::endl;
  });
  QObject::connect(signProvider, &TbaiSignProcess::finished, []()
  {
    signProvider->deleteLater();
  });
  signProvider.sign(invoice);
}
```

##### Catching errors
Errors might happen during the signing process. You should also listen to the `failure` signal
so you can report the errors:

```
QObject::connect(signProcess, &TbaiSignProcess::failed, [](QString error)
{
  qDebug() << error;
});
```

##### Signature
As you will need to use the generated signature to properly sign the next invoices, it may be a good
idea to store the signature someplace easily accessible for your signing process. When signing an invoice,
on top of intercepting the generated invoice XML, you can also intercept the signature using the `generatedSignature`
signal:

```
QObject::connect(signProcess, &TbaiSignProcess::generatedSignature, [](QByteArray signature)
{
  // In this handler, you're expected to store the signature, so that from now on, the
  // `getSignature` virtual method our `invoice` object returns the value of `signature`.
  qDebug() << "New TicketBAI signature:" << signature;
});
```

Your `TbaiInvoiceInterface` implementation *must* return the value of `signature` when its `getSignature`
method gets called. This will be relevant when signing the next invoice.

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
