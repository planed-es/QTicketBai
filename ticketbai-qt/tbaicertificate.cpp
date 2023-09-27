#include "tbaicertificate.h"
#include <QRegularExpression>
#include <QFile>
#include <QProcess>
#include <QSslCertificate>
#include <QSslKey>
#include <QDebug>

TbaiCertificate::TbaiCertificate(QObject* parent) : QObject(parent)
{
  connect(this, &TbaiCertificate::pathChanged,     this, &TbaiCertificate::refresh);
  connect(this, &TbaiCertificate::passwordChanged, this, &TbaiCertificate::refresh);
}

TbaiCertificate::~TbaiCertificate()
{
  _pemCertificateFile.remove();
  _pemKeyFile.remove();
}

void TbaiCertificate::refresh()
{
  if (!_path.isEmpty())
  {
    _prepared = false;
    emit readyChanged();
    prepare();
  }
}

QString TbaiCertificate::pemCertificatePath() const
{
  return _pemCertificateFile.fileName();
}

QString TbaiCertificate::pemKeyPath() const
{
  return _pemKeyFile.fileName();
}

static QStringList certificatePrepareParams(TbaiCertificate& certificate)
{
  return QStringList()
    << "pkcs12"
    << "-in" << certificate.path()
    << "-passin" << ("pass:" + certificate.password());
}

bool TbaiCertificate::prepare()
{
  if (!_prepared)
  {
    QFile pfxFile(path());

    // Prepare pem temporary files
    _pemCertificateFile.open(); _pemCertificateFile.close();
    _pemKeyFile.open(); _pemKeyFile.close();
    // Load PKCS12 certificate
    if (pfxFile.open(QIODevice::ReadOnly))
    {
      bool a = preparePemCertificates();
      bool b = QSslCertificate::importPkcs12(&pfxFile, &_sslKey, &_certificate, nullptr, password().toUtf8());

      if (!a)
        qDebug() << "TbaiCertificate: failed to generate certificate and key in PEM format from" << TbaiCertificate::path();
      if (!b)
        qDebug() << "TbaiCertificate: QSslCertificate failed to import pkcs12 certificate" << TbaiCertificate::path();
      _prepared = a && b;
      emit readyChanged();
    }
    else
      qDebug() << "TbaiCertificate: cannot open file" << path();
  }
  return _prepared;
}

bool TbaiCertificate::preparePemCertificates()
{
  QProcess pem_process, key_process;
  QStringList pem_params = certificatePrepareParams(*this);
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

QByteArray TbaiCertificate::digest(QCryptographicHash::Algorithm algorithm)
{
  return _certificate.digest(algorithm);
}
