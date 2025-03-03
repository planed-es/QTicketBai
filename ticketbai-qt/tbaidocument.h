#ifndef  TBAIDOCUMENT_H
# define TBAIDOCUMENT_H

# include "abstracttbaidocument.h"
# include <QDate>

class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT TbaiDocument : public AbstractTbaiDocument
{
public:
  TbaiDocument();

  QByteArray documentElementType() const override { return "TicketBai"; }
  QByteArray documentXmlns()       const override { return "urn:ticketbai:emision"; }
  QByteArray invoiceNumber() const;
  QByteArray invoiceSerie() const;
  QDate      invoiceDate() const;

  TbaiDocument&  createFrom(const TbaiInvoiceInterface&);

  static QString getFileNameFor(const TbaiInvoiceInterface&);

protected:
  QDomElement invoiceHeader() const;
};

#endif // TBAIDOCUMENT_H
