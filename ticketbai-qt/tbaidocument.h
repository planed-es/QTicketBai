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
  bool           loadFromFile(const QString& path);
  bool           loadFrom(const QByteArray& xml);
  bool           loadFrom(const QString& xml) { return loadFrom(xml.toUtf8()); }
  void           appendSignature(const QDomElement&);
  bool           isSigned() const;
  QByteArray     signature() const;

  static QString getFileNameFor(const TbaiInvoiceInterface&);
  static QString signatureNamespace();
};

#endif // TBAIDOCUMENT_H
