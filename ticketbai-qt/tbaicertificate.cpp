#include "tbaicertificate.h"
#include <QRegularExpression>
#include <QFile>
#include <QProcess>
#include <QSslCertificate>
#include <QSslKey>
#include <QTemporaryFile>
#include <QDebug>

QSslCertificate TbaiCertificate::certificate;
QSslKey         TbaiCertificate::sslKey;
static QTemporaryFile pemCertificateFile;
static QTemporaryFile pemKeyFile;

QString TbaiCertificate::alias()
{
  QString alias = qgetenv("TBAI_CERTIFICATE_ALIAS");

  return alias.isEmpty() ? QString("1") : alias;
}

QString TbaiCertificate::pemCertificatePath()
{
  return pemCertificateFile.fileName();
}

QString TbaiCertificate::pemKeyPath()
{
  return pemKeyFile.fileName();
}

static QStringList certificate_prepare_params()
{
  return QStringList()
    << "pkcs12"
    << "-in" << TbaiCertificate::path()
    << "-passin" << ("pass:" + TbaiCertificate::password());
}

bool TbaiCertificate::prepare()
{
  QFile pfxFile(TbaiCertificate::path());

  // Prepare pem temporary files
  pemCertificateFile.open(); pemCertificateFile.close();
  pemKeyFile.open(); pemKeyFile.close();
  // Load PKCS12 certificate
  if (pfxFile.open(QIODevice::ReadOnly))
  {
    bool a = preparePemCertificates();
    bool b = QSslCertificate::importPkcs12(&pfxFile, &sslKey, &certificate, nullptr, TbaiCertificate::password().toUtf8());

    if (!a)
      qDebug() << "TbaiCertificate: failed to generate certificate and key in PEM format from" << TbaiCertificate::path();
    if (!b)
      qDebug() << "TbaiCertificate: QSslCertificate failed to import pkcs12 certificate" << TbaiCertificate::path();
    return a && b;
  }
  else
    qDebug() << "TbaiCertificate: cannot open file" << TbaiCertificate::path();
  return false;
}

bool TbaiCertificate::preparePemCertificates()
{
  QProcess pem_process, key_process;
  QStringList pem_params = certificate_prepare_params();
  QStringList key_params = pem_params;
  QString openssl_bin("openssl");

  pem_params << "-out" << pemCertificatePath();
  pem_params << "-clcerts" << "-nokeys";
  pem_process.start(openssl_bin, pem_params);
  key_params << "-out" << pemKeyPath();
  key_params << "-nocerts" << "-nodes";
  key_process.start(openssl_bin, key_params);
  pem_process.waitForFinished();
  key_process.waitForFinished();
  if (pem_process.exitCode())
    qDebug() << "certificate pem:" << pem_process.readAllStandardError();
  if (key_process.exitCode())
    qDebug() << "certificate key:" << key_process.readAllStandardError();
  return (pem_process.exitCode() + key_process.exitCode()) == 0;
}

void TbaiCertificate::cleanup()
{
  pemCertificateFile.remove();
  pemKeyFile.remove();
}

QByteArray TbaiCertificate::digest(QCryptographicHash::Algorithm algorithm)
{
  return certificate.digest(algorithm);
}
