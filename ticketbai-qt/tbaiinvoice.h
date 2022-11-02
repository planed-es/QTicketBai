#ifndef  TBAIINVOICE_H
# define TBAIINVOICE_H

# include "ticketbai-qt_global.h"
# include "invoiceinterface.h"
# include <QtXml>

class TICKETBAIQT_EXPORT TbaiInvoice
{
public:
  TbaiInvoice(const TbaiInvoiceInterface* ptr) : invoice(*ptr) {}
  TbaiInvoice(const TbaiInvoiceInterface& ref) : invoice(ref) {}

  QByteArray getId() const;
  QByteArray getIdWithCRC() const;
  QUrl       getUrl() const;

private:
  QByteArray   generateCRC(const QByteArray& input) const;

  const TbaiInvoiceInterface& invoice;
};

#endif // TBAIINVOICE_H
