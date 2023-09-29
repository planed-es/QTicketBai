#ifndef  INVOICELINE_H
# define INVOICELINE_H

# include <QObject>
# include "product.h"

class InvoiceLine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString  label          MEMBER _label         NOTIFY labelChanged)
  Q_PROPERTY(double   unitPrice      MEMBER _unitPrice     NOTIFY amountChanged)
  Q_PROPERTY(double   quantity       MEMBER _quantity      NOTIFY amountChanged)
  Q_PROPERTY(double   taxRate        MEMBER _taxRate       NOTIFY amountChanged)
  Q_PROPERTY(double   total          READ   total          NOTIFY amountChanged)
  Q_PROPERTY(double   totalWithTaxes READ   totalWithTaxes NOTIFY amountChanged)
  Q_PROPERTY(Product* product READ product WRITE setProduct NOTIFY productChanged)
public:
  explicit InvoiceLine(QObject* parent = nullptr);

  QString label() const { return _label; }
  double unitPrice() const { return _unitPrice; }
  double quantity() const { return _quantity; }
  double taxRate() const { return _taxRate; }
  double total() const;
  double totalWithTaxes() const;
  Product* product() const { return _product; }
  void setProduct(Product*);

signals:
  void labelChanged();
  void amountChanged();
  void productChanged();

private slots:
  void onProductDeleted(QObject*);

private:
  QString _label;
  Product* _product;
  double _unitPrice;
  double _quantity;
  double _taxRate;
};

#endif
