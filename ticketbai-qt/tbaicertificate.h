#ifndef  TBAICERTIFICATE_H
# define TBAICERTIFICATE_H

# include <QObject>
# include <QByteArray>
# include <QString>
# include <QSslConfiguration>
# include <QSslCertificate>
# include <QSslKey>
# include <QCryptographicHash>
# include <QTemporaryFile>
# include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT TbaiCertificate : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString path     MEMBER _path     NOTIFY pathChanged)
  Q_PROPERTY(QString password MEMBER _password NOTIFY passwordChanged)
  Q_PROPERTY(bool    ready    READ   isReady   NOTIFY readyChanged)
public:
  TbaiCertificate(QObject* parent = nullptr);
  ~TbaiCertificate();

  TbaiCertificate& withPath(const QString& value) { _path = value; return *this; }
  TbaiCertificate& withPassword(const QString& value) { _password = value; return *this; }

  const QString& path() const { return _path; }
  const QString& password() const { return _password; }
  const QSslCertificate& certificate() const { return _certificate; }
  const QSslKey& sslKey() const { return _sslKey; }

  void setPath(const QString& value) { _path = value; emit pathChanged(); }
  void setPassword(const QString& value) { _password = value; emit passwordChanged(); }

  bool prepare();
  bool isReady() const { return _prepared; }
  QString pemCertificatePath() const;
  QString pemKeyPath() const;
  QString serialNumber();
  QByteArray digest(QCryptographicHash::Algorithm);

signals:
  void pathChanged();
  void passwordChanged();
  void readyChanged();

private slots:
  void refresh();

private:
  bool preparePemCertificates();

  QString         _path, _password;
  QSslCertificate _certificate;
  QSslKey         _sslKey;
  QTemporaryFile  _pemCertificateFile;
  QTemporaryFile  _pemKeyFile;
  bool            _prepared = false;
};

#endif // TBAICERTIFICATE_H
