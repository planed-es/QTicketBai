#ifndef  TBAIINVOICE_H
# define TBAIINVOICE_H

# include "invoiceinterface.h"
# include <QtXml>

class TbaiInvoice
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
