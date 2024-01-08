#ifndef  TBAISIGNPROCESS_H
# define TBAISIGNPROCESS_H

# include <QObject>
# include <QProcess>
# include <QTemporaryFile>
# include "abstracttbaidocument.h"
# include "context.h"

class QProcess;
class QDomDocument;
class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT TbaiSignProcess
{
public:
  static bool checkSettings(const TbaiContext&);

  struct ReturnValue
  {
    ReturnValue(QString param = QString()) : error(param) {}
    operator bool() const { return error.isNull(); }
    QString error;
    QByteArray xml, signature;
  };

  static ReturnValue sign(AbstractTbaiDocument&);
  static ReturnValue sign(const TbaiCertificate&, AbstractTbaiDocument&);
};

#endif // TBAISIGNPROCESS_H
