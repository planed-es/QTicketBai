#ifndef  TBAICANCELDOCUMENT_H
# define TBAICANCELDOCUMENT_H

# include "uploaddocument.h"
# include "abstracttbaidocument.h"

class TICKETBAIQT_EXPORT LROECancelDocument : public LROEUploadDocument
{
public:
  explicit LROECancelDocument(ModelType);
};

class TICKETBAIQT_EXPORT TbaiCancelDocument : public AbstractTbaiDocument
{
public:
  explicit TbaiCancelDocument();

  QByteArray documentElementType() const override { return "AnulaTicketBai"; }
  QByteArray documentXmlns()       const override { return "urn:ticketbai:anulacion"; }

  TbaiCancelDocument& createFrom(const TbaiInvoiceInterface&);
};

#endif
