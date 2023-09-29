#include "product.h"
#include <QJsonDocument>
#include <QJsonObject>

Product::Product(QObject* parent) : QObject(parent)
{
  _defaultPrice = _defaultVat = 0;
  _vatExemption = TbaiInvoiceInterface::NoVatExemption;
  _vatState     = TbaiInvoiceInterface::SubjectToVat;
}

QString Product::toString() const
{
  QJsonObject object;

  object["name"]         = _name;
  object["defaultPrice"] = _defaultPrice;
  object["defaultVat"]   = _defaultVat;
  object["vatExemption"] = static_cast<int>(_vatExemption);
  object["vatState"]     = static_cast<int>(_vatState);
  return QString(QJsonDocument(object).toJson());
}

void Product::fromString(const QString& source)
{
  QJsonDocument document = QJsonDocument::fromJson(source.toUtf8());

  _name         = document["name"].toString();
  _defaultPrice = document["defaultPrice"].toDouble();
  _defaultVat   = document["defaultVat"].toDouble();
  _vatExemption = static_cast<TbaiInvoiceInterface::VatExemption>(document["vatExemption"].toInt(0));
  _vatState     = static_cast<TbaiInvoiceInterface::NotSubjectToVatReason>(document["vatState"].toInt(0));
  emit changed();
}
