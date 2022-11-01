#ifndef  TBAICERTIFICATE_H
# define TBAICERTIFICATE_H

# include <QByteArray>
# include <QString>
# include <QSslConfiguration>
# include <QSslCertificate>
# include <QCryptographicHash>

struct TbaiCertificate
{
public:
  static QSslCertificate certificate;

  static QSslKey         sslKey;

  static QString alias();

  static QString path()
  {
    return qgetenv("TBAI_CERTIFICATE_PATH");
  }

  static QString password()
  {
    return qgetenv("TBAI_CERTIFICATE_PASSWORD");
  }

  static bool prepare();
  static void cleanup();
  static QString pemPath();
  static QString keyPath();
  static QString serialNumber();
  static QByteArray digest(QCryptographicHash::Algorithm);

private:
  static bool preparePemCertificates();
  static bool extractMetadata();
  static QString pathWithExtension(const QString& extension);
};

#endif // TBAICERTIFICATE_H
