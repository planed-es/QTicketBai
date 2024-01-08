#ifndef  TBAIDOCUMENT_H
# define TBAIDOCUMENT_H

# include "abstracttbaidocument.h"

class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT TbaiDocument : public AbstractTbaiDocument
{
public:
  TbaiDocument();

  QByteArray documentElementType() const override { return "TicketBai"; }
  QByteArray documentXmlns()       const override { return "urn:ticketbai:emision"; }

  TbaiDocument&  createFrom(const TbaiInvoiceInterface&);

  static QString getFileNameFor(const TbaiInvoiceInterface&);
};

#endif // TBAIDOCUMENT_H
