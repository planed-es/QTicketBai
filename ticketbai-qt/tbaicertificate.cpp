#include "tbaicertificate.h"
#include <QRegularExpression>
#include <QFile>
#include <QProcess>
#include <QSslCertificate>
#include <QSslKey>
#include <QDebug>

static QString certificateIssuer;
static QString certificateSerialNumber;

QString TbaiCertificate::alias()
{
  QString alias = qgetenv("TBAI_CERTIFICATE_ALIAS");

  return alias.isEmpty() ? QString("1") : alias;
}

QString TbaiCertificate::pemPath()
{
  return pathWithExtension(".pem");
}

QString TbaiCertificate::keyPath()
{
  return pathWithExtension(".key");
}

QString TbaiCertificate::pathWithExtension(const QString& extension)
{
  QString            certificatePath = path();
  QRegularExpression extensionMatcher("\\.p12$", QRegularExpression::CaseInsensitiveOption);

  return certificatePath.replace(extensionMatcher, extension);
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
  bool a = preparePemCertificates();
  bool b = extractMetadata();

  return a && b;
}

bool TbaiCertificate::preparePemCertificates()
{
  QProcess pem_process, key_process;
  QStringList pem_params = certificate_prepare_params();
  QStringList key_params = pem_params;
  QString openssl_bin("openssl");

  pem_params << "-out" << TbaiCertificate::pathWithExtension(".pem");
  pem_params << "-clcerts" << "-nokeys";
  pem_process.start(openssl_bin, pem_params);
  key_params << "-out" << TbaiCertificate::pathWithExtension(".key");
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

bool TbaiCertificate::extractMetadata()
{
  QProcessEnvironment keytoolEnv;
  QProcess keytoolProcess;
  QStringList keytoolParams;
  QString keytoolBin("keytool");

  keytoolEnv.insert("LANG", "en_US.UTF-8");
  keytoolProcess.setProcessEnvironment(keytoolEnv);
  keytoolParams
    << "-v" << "-list"
    << "-storetype" << "pkcs12"
    << "-keystore" << TbaiCertificate::path()
    << "-storepass" << TbaiCertificate::password();
  keytoolProcess.start(keytoolBin, keytoolParams);
  keytoolProcess.waitForFinished();
  for (const QByteArray& line : keytoolProcess.readAllStandardOutput().split('\n'))
  {
    qDebug() << line;
    if (line.startsWith("Issuer: "))
      certificateIssuer = line.sliced(8);
    else if (line.startsWith("Serial number: "))
      certificateSerialNumber = line.sliced(15);
  }
  if (certificateIssuer.length() == 0 || certificateSerialNumber.length() == 0)
  {
    qDebug() << "TbaiCertificate: failed to extract issuer name and serial number from" << TbaiCertificate::path();
    return false;
  }
  return true;
}

QString TbaiCertificate::issuerName()
{
  return certificateIssuer;
}

QString TbaiCertificate::serialNumber()
{
  return certificateSerialNumber;
}

void TbaiCertificate::cleanup()
{
  QFile::remove(TbaiCertificate::pathWithExtension(".pem"));
  QFile::remove(TbaiCertificate::pathWithExtension(".key"));
}
