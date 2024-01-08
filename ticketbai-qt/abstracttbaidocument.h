#ifndef  ABSTRACTTBAIDOCUMENT_H
# define ABSTRACTTBAIDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>
# include "ticketbai-qt_global.h"

# define TBAI_VERSION          "1.2"
# define TBAI_SOFTWARE_VERSION "1.0"

class TbaiInvoiceInterface;
class TbaiContext;

class TICKETBAIQT_EXPORT AbstractTbaiDocument : public QDomDocument
{
public:
  virtual QByteArray documentElementType() const = 0;
  virtual QByteArray documentXmlns() const = 0;

  void useContext(const TbaiContext& value) { _context = &value; }

  bool loadFrom(const QByteArray& xml);
  bool loadFrom(const QString& xml) { return loadFrom(xml.toUtf8()); }
  bool loadFromFile(const QString&);

  void appendSignature(const QDomElement&);
  bool isSigned() const;
  QByteArray signature() const;
  static QString signatureNamespace();

protected:
  const TbaiContext& context() const;
  void prepareDocument();
  QDomElement generateFingerprint(const TbaiInvoiceInterface&);
  QDomElement generateEmitter();

  QDomElement root, headerEl;
  const TbaiContext* _context = nullptr;
  bool includePreviousInvoiceInFingerprint = true;
};

#endif
