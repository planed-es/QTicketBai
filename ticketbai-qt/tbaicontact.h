#ifndef  TBAIQT_QCOMPANYDATA_H
# define TBAIQT_QCOMPANYDATA_H

# include <QObject>
# include "companydata.h"
# include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT TbaiContact : public QObject, private CompanyData
{
  Q_OBJECT

  Q_PROPERTY(QString          name        READ name WRITE setName NOTIFY changed)
  Q_PROPERTY(QString          address     READ address WRITE setAddress NOTIFY changed)
  Q_PROPERTY(QString          city        READ city WRITE setCity NOTIFY changed)
  Q_PROPERTY(TbaiIdentityType idType      READ idType WRITE setIdType NOTIFY changed)
  Q_PROPERTY(QString          id          READ id WRITE setId NOTIFY changed)
  Q_PROPERTY(QString          phone       READ phone WRITE setPhone NOTIFY changed)
  Q_PROPERTY(QString          fax         READ fax WRITE setFax NOTIFY changed)
  Q_PROPERTY(QString          postalCode  READ postalCode WRITE setPostalCode NOTIFY changed)
  Q_PROPERTY(QString          email       READ email WRITE setEmail NOTIFY changed)
  Q_PROPERTY(QByteArray       countryCode READ countryCode WRITE setCountryCode NOTIFY changed)
public:
  TbaiContact(QObject* parent = nullptr);

  TbaiContact& fromData(const CompanyData&);
  TbaiContact& operator=(const CompanyData& d) { return fromData(d); }
  const CompanyData& asCompanyData() const { return *this; }

  const QString& name() const { return CompanyData::name; }
  const QString& address() const { return CompanyData::address; }
  const QString& city() const { return CompanyData::city; }
  TbaiIdentityType idType() const { return CompanyData::idType; }
  const QString& id() const { return CompanyData::id; }
  const QString& phone() const { return CompanyData::phone; }
  const QString& fax() const { return CompanyData::fax; }
  const QString& postalCode() const { return CompanyData::postalCode; }
  const QString& email() const { return CompanyData::email; }
  const QByteArray& countryCode() const { return CompanyData::countryCode; }

  void setName(const QString& value) { CompanyData::name = value; emit changed(); }
  void setAddress(const QString& value) { CompanyData::address = value; emit changed(); }
  void setCity(const QString& value) { CompanyData::city = value; emit changed(); }
  void setIdType(TbaiIdentityType value) { CompanyData::idType = value; emit changed(); }
  void setId(const QString& value) { CompanyData::id = value; emit changed(); }
  void setPhone(const QString& value) { CompanyData::phone = value; emit changed(); }
  void setFax(const QString& value) { CompanyData::fax = value; emit changed(); }
  void setPostalCode(const QString& value) { CompanyData::postalCode = value; emit changed(); }
  void setEmail(const QString& value) { CompanyData::email = value; emit changed(); }
  void setCountryCode(const QByteArray& value) { CompanyData::countryCode = value; emit changed(); }

signals:
  void changed();
};

#endif
