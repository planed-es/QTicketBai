#ifndef  INVOICECONTROLLER_H
# define INVOICECONTROLLER_H

# include "invoice.h"

class TbaiContext;

class InvoiceController : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<Invoice> list READ qmlList NOTIFY listChanged)
public:
  static InvoiceController* self;

  InvoiceController(const TbaiContext& context, QObject* = nullptr);
  ~InvoiceController();

  Q_INVOKABLE Invoice* createInvoice();
  Q_INVOKABLE void remove(Invoice*);
  Q_INVOKABLE bool sign(Invoice*);

  Invoice* previousInvoice(const Invoice*) const;

  QQmlListProperty<Invoice> qmlList() { return QQmlListProperty<Invoice>(this, &_invoices); }

signals:
  void listChanged();
  void cantRemoveSignedInvoice();

private:
  const TbaiContext& _context;
  QList<Invoice*> _invoices;
};

#endif
