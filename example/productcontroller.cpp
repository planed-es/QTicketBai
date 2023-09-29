#include "productcontroller.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>

ProductController::ProductController(QObject* parent) : QObject(parent)
{
  QSettings settings("ticketbai-qt", "example");
  QJsonDocument document = QJsonDocument::fromJson(settings.value("products").toString().toUtf8());

  if (document.isArray())
  {
    for (const QJsonValue& value : document.array())
      createProduct()->fromString(value.toString());
  }
}

ProductController::~ProductController()
{
  save();
}

void ProductController::save()
{
  QSettings settings("ticketbai-qt", "example");
  QJsonArray jsonList;

  for (const auto* product : _products)
    jsonList << product->toString();
  settings.setValue("products", QJsonDocument(jsonList).toJson());
}

Product* ProductController::createProduct()
{
  Product* product = new Product(this);

  _products << product;
  emit listChanged();
  return product;
}

void ProductController::remove(Product* product)
{
  if (product)
  {
    _products.removeAll(product);
    product->deleteLater();
    emit listChanged();
  }
}
