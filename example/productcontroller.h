#ifndef  PRODUCTCONTROLLER_H
# define PRODUCTCONTROLLER_H

# include "product.h"
# include <QQmlListProperty>

class ProductController : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<Product> list READ qmlList NOTIFY listChanged)
public:
  ProductController(QObject* parent = nullptr);
  ~ProductController();

  Q_INVOKABLE Product* createProduct();
  Q_INVOKABLE void remove(Product*);
  Q_INVOKABLE void save();

  QQmlListProperty<Product> qmlList() { return QQmlListProperty<Product>(this, &_products); }

signals:
  void listChanged();

private:
  QList<Product*> _products;
};

#endif
