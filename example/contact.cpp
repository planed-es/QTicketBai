#include "contact.h"
#include <QJsonDocument>
#include <QJsonObject>

Contact::Contact(QObject* parent) : TbaiContact(parent)
{
}

QString Contact::toString() const
{
  QJsonObject object;

  object["name"] = name();
  object["address"] = address();
  object["city"] = city();
  object["idType"] = static_cast<int>(idType());
  object["id"] = id();
  object["phone"] = phone();
  object["fax"] = fax();
  object["postalCode"] = postalCode();
  object["email"] = email();
  object["countryCode"] = QString(countryCode());
  return QString(QJsonDocument(object).toJson());
}

void Contact::fromString(const QString& source)
{
  QJsonDocument document = QJsonDocument::fromJson(source.toUtf8());

  setName       (document["name"].toString());
  setAddress    (document["address"].toString());
  setCity       (document["city"].toString());
  setIdType     (static_cast<TbaiIdentityType>(document["idType"].toInt(0)));
  setId         (document["id"].toString());
  setPhone      (document["phone"].toString());
  setFax        (document["fax"].toString());
  setPostalCode (document["postalCode"].toString());
  setEmail      (document["email"].toString());
  setCountryCode(document["countryCode"].toString().toUtf8());
}
