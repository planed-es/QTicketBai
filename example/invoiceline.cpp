#include "invoiceline.h"

InvoiceLine::InvoiceLine(QObject* parent) : QObject(parent)
{
  _product      = nullptr;
  _unitPrice    = _quantity = _taxRate = 0;
}

void InvoiceLine::setProduct(Product* product)
{
  if (_product)
    disconnect(_product, &QObject::destroyed, this, &InvoiceLine::onProductDeleted);
  _product = product;
  if (_product)
  {
    connect(_product, &QObject::destroyed, this, &InvoiceLine::onProductDeleted); 
    _label = _product->name();
    _unitPrice = _product->defaultPrice();
    _taxRate = _product->defaultVat();
    emit amountChanged();
  }
}

void InvoiceLine::onProductDeleted(QObject* obj)
{
  if (obj == _product)
  {
    _product = nullptr;
    emit productChanged();
  }
}

double InvoiceLine::total() const
{
  return _unitPrice * _quantity;
}

double InvoiceLine::totalWithTaxes() const
{
  return total() * (1 + (_taxRate / 100));
}
