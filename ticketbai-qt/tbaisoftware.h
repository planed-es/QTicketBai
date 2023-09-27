#ifndef  TBAIQT_SOFTWARE_H
# define TBAIQT_SOFTWARE_H

# include <QObject>
# include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT TbaiSoftware : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString license MEMBER _license NOTIFY changed)
  Q_PROPERTY(QString cif MEMBER _cif NOTIFY changed)
  Q_PROPERTY(QString name MEMBER _name NOTIFY changed)
  Q_PROPERTY(QByteArray deviceUid MEMBER _deviceUid NOTIFY changed)
public:
  TbaiSoftware(QObject* parent = nullptr);

  TbaiSoftware& withLicense(const QString& value) { _license = value; return *this; }
  TbaiSoftware& withCif(const QString& value) { _cif = value; return *this; }
  TbaiSoftware& withName(const QString& value) { _name = value; return *this; }
  TbaiSoftware& withDeviceUid(const QByteArray& value) { _deviceUid = value; return *this; }

  const QString& license() const { return _license; }
  const QString& cif() const { return _cif; }
  const QString& name() const { return _name; }
  const QByteArray& deviceUid() const { return _deviceUid; }

  void setLicense(const QString& value) { _license = value; emit changed(); }
  void setCif(const QString& value) { _cif = value; emit changed(); }
  void setName(const QString& value) { _name = value; emit changed(); }
  void setDeviceUid(const QByteArray& value) { _deviceUid = value; emit changed(); }

signals:
  void changed();
private:
  QString _license, _cif, _name;
  QByteArray _deviceUid;
};

#endif
