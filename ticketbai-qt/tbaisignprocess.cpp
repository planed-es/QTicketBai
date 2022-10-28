#include <QProcess>
#include <QTemporaryFile>
#include <QStandardPaths>
#include "tbaisignprocess.h"
#include "tbaidocument.h"
#include "tbaicertificate.h"
#include <iostream>
#include <QDebug>

static QString findAutofirma()
{
  QString exeName = "autofirma.jar";
  QString path = QStandardPaths::findExecutable(exeName);

  return path.isEmpty() ? (qgetenv("AUTOFIRMA_PATH") + '/' + exeName) : path;
}

TbaiSignProcess::TbaiSignProcess(QObject *parent) : QObject(parent)
{
  connect(this, &TbaiSignProcess::failed,       this, &TbaiSignProcess::finished, Qt::QueuedConnection);
  connect(this, &TbaiSignProcess::generatedXml, this, &TbaiSignProcess::finished, Qt::QueuedConnection);
}

bool TbaiSignProcess::checkSettings()
{
  QByteArray certificate = qgetenv("TBAI_CERTIFICATE_PATH");

#define TBAI_FILE_CHECKER(path) \
  if (!path.isEmpty() && QFile::exists(path)) \
    std::cerr << "using " << path.toStdString() << std::endl; \
  else \
  { \
    std::cerr << "not found." << std::endl; \
    return false; \
  }

  std::cerr << "Checking configuration for QTicketBAI:" << std::endl;
  std::cerr << "- Looking for java runtime:\t";
  TBAI_FILE_CHECKER(QStandardPaths::findExecutable("java"))
  std::cerr << "- Looking for autofirma.jar:\t";
  TBAI_FILE_CHECKER(findAutofirma())
  std::cerr << "- Looking for certificate:\t";
  TBAI_FILE_CHECKER(TbaiCertificate::path())
  std::cerr << "- Certificate password ?\t" << (TbaiCertificate::password().isEmpty() ? "No" : "Yes") << std::endl;
  std::cerr << "- Using TicketBAI software CIF:\t" << qgetenv("TBAI_SOFTWARE_CIF").toStdString() << std::endl;
  std::cerr << "- Using TicketBAI software:\t" << qgetenv("TBAI_SOFTWARE_NAME").toStdString() << std::endl;
  std::cerr << "- Using TicketBAI Tax Authority:\t" << qgetenv("TBAI_TAX_AUTHORITY_URL").toStdString() << std::endl;
  return true;
}

void TbaiSignProcess::sign(const TbaiInvoiceInterface& invoice)
{
  qDebug() << "TbaiSignProcess";
  document.createFrom(invoice);
  if (inputFile.open() && outputFile.open())
  {
    autofirma = new QProcess(this);
    connect(autofirma,
      QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      this,
      &TbaiSignProcess::onAutofirmaDone
    );
    connect(autofirma, &QProcess::errorOccurred, this, &TbaiSignProcess::onAutofirmaFailed);
    inputFile.write(document.toByteArray());
    inputFile.close();
    outputFile.close();
    launchAutofirma();
  }
  else
    emit failed("could not open temporary files");
}

void TbaiSignProcess::launchAutofirma()
{
  QString javaPath            = QStandardPaths::findExecutable("java");
  QString autofirmaPath       = findAutofirma();
  QStringList params;

  params << "-jar" << autofirmaPath << "sign"
         << "-format"   << "xades"
         << "-i"        << inputFile.fileName()
         << "-o"        << outputFile.fileName()
         << "-store"    << ("pkcs12:" + TbaiCertificate::path())
         << "-alias"    << TbaiCertificate::alias();
  if (!TbaiCertificate::password().isEmpty())
    params << "-password" << TbaiCertificate::password();
  qDebug() << "TbaiSignProcess::launchAutofirma" << javaPath << params;
  autofirma->start(javaPath, params, QIODevice::ReadOnly);
}

bool TbaiSignProcess::wait()
{
  autofirma->waitForStarted();
  autofirma->waitForFinished();
  return autofirma->state() == QProcess::NotRunning && autofirma->exitCode() == 0;
}

void TbaiSignProcess::onAutofirmaFailed(QProcess::ProcessError error)
{
  qDebug() << "TbaiSignProcess::onAutofirmaFailed" << error;
  switch (error)
  {
  case QProcess::FailedToStart:
    emit failed("autofirma process failed to start");
    break ;
  default:
    emit failed("autofirma process failed for unknown reasons");
    break ;
  }
}

void TbaiSignProcess::onAutofirmaDone(int code, QProcess::ExitStatus)
{
  qDebug() << "TbaiSignProcess::onAutofirmaDone" << code;
  if (code == 0)
  {
    QDomDocument signatureDocument;

    outputFile.open();
    signatureDocument.setContent(outputFile.readAll());
    outputFile.close();
    generateXml(signatureDocument);
  }
  else
    emit failed("autofirma.jar returned with satus " + QString::number(code));
}

void TbaiSignProcess::generateXml(const QDomDocument &xmlSignature)
{
  QDomElement signatureEl = xmlSignature.firstChildElement("ds:Signature");

  document.appendSignature(signatureEl);
  emit generatedSignature(document.getSignature());
  emit generatedDocument(document);
  emit generatedXml(document.toByteArray(2));
}
