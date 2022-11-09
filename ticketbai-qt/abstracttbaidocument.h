#ifndef  ABSTRACTTBAIDOCUMENT_H
# define ABSTRACTTBAIDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>
# include "ticketbai-qt_global.h"

# define TBAI_VERSION          "1.2"
# define TBAI_SOFTWARE_VERSION "1.0"

class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT AbstractTbaiDocument : public QDomDocument
{
public:
  virtual QByteArray documentElementType() const = 0;
  virtual QByteArray documentXmlns() const = 0;

protected:
  void prepareDocument();
  QDomElement generateFingerprint(const TbaiInvoiceInterface&);
  QDomElement generateEmitter();

  QDomElement root, headerEl;
};

#endif
