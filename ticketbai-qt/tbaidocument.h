#ifndef  TBAIDOCUMENT_H
# define TBAIDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>
# include "ticketbai-qt_global.h"

class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT TbaiDocument : public QDomDocument
{
public:
  TbaiDocument();

  TbaiDocument&  createFrom(const TbaiInvoiceInterface&);
  bool           loadFromFile(const QString& path);
  bool           loadFrom(const QByteArray& xml);
  bool           loadFrom(const QString& xml) { return loadFrom(xml.toUtf8()); }
  void           appendSignature(const QDomElement&);
  bool           isSigned() const;
  QByteArray     signature() const;

  static QString getFileNameFor(const TbaiInvoiceInterface&);
  static QString signatureNamespace();

private:
  QDomElement root;
};

#endif // TBAIDOCUMENT_H
