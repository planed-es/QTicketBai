#ifndef  PRODUCT_H
# define PRODUCT_H

# include <QObject>
# include <ticketbai-qt/invoiceinterface.h>

class Product : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name         MEMBER _name         NOTIFY changed)
  Q_PROPERTY(double  defaultPrice MEMBER _defaultPrice NOTIFY changed)
  Q_PROPERTY(double  defaultVat   MEMBER _defaultVat   NOTIFY changed)
  Q_PROPERTY(TbaiInvoiceDefines::VatExemption          vatExemption   MEMBER _vatExemption  NOTIFY changed)
  Q_PROPERTY(TbaiInvoiceDefines::NotSubjectToVatReason vatState       MEMBER _vatState      NOTIFY changed)
public:
  explicit Product(QObject* parent = nullptr);

  QString toString() const;
  void fromString(const QString&);

  const QString& name() const { return _name; }
  double defaultPrice() const { return _defaultPrice; }
  double defaultVat() const { return _defaultVat; }
  TbaiInvoiceDefines::VatExemption vatExemption() const { return _vatExemption; }
  TbaiInvoiceDefines::NotSubjectToVatReason vatState() const { return _vatState; }

signals:
  void changed();

private:
  QString _name;
  double  _defaultPrice;
  double  _defaultVat;
  TbaiInvoiceInterface::VatExemption          _vatExemption;
  TbaiInvoiceInterface::NotSubjectToVatReason _vatState;
};

#endif
