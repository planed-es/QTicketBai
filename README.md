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

```
#include <ticketbai-qt/tbaisignprocess.h>
#include <QFile>

static void sign_invoice(const TbaiInvoiceInterface& invoice, std::function<void()> done)
{
  auto* signProcess = new TbaiSignProcess();

  //
  // Listen to signals
  //

  // The `finished` signal triggers at the end of the process, regardless of whether it succeeded or not. 
  QObject::connect(signProcess, &TbaiSignProcess::finished, [signProcess]() { signProcess->deleteLater(); });

  // The `failed` signal triggers at the end of the process to report an error when generating the TicketBAI invoice:
  QObject::connect(signProcess, &TbaiSignProcess::failed, [](QString error) { qDebug() << error; });

  // The `generatedSignature` triggers when the TicketBAI signature is available, allowing you to store the
  // generated signature, which will be needed when generating the next invoices:
  QObject::connect(signProcess, &TbaiSignProcess::generatedSignature, [](QByteArray signature)
  {
    // In this handler, you're expected to store the signature, so that from now on, the
    // `getSignature` virtual method our `invoice` object returns the value of `signature`.
    qDebug() << "New TicketBAI signature:" << signature;
  });

  // The `generatedXml` triggers when the signing process has ended for the current invoice, and gives you
  // the opportunity to save the invoice's associated XML document, which will be needed when submitting
  // your invoices to LROE.
  QObject::connect(signProcess, &TbaiSignProcess::generatedXml, [](QByteArray xml)
  {
    QFile output("/tmp/signed-invoice.xml");
    if (output.open(QIODevice::WriteOnly))
      output << xml;
  });

  //
  // Start the signing process
  //
  signerProcess->sign(invoice);

  // Note that this process is asynchronous: the `sign` method won't wait for the signing process to
  // end before returning.
}
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

## Running the tests

The tests use the example and certificate sets provided by in the
[technical documentation](https://www.batuz.eus/es/documentacion-tecnica) for
the LROE process.

The `test.sh` script is provided to download the test environment and run all the tests.
It is designed to run from the your build directory:

```
mkdir build
cd build
cmake ..
make
../test.sh
```
