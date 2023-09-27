#ifndef  TBAISIGNPROCESS_H
# define TBAISIGNPROCESS_H

# include <QObject>
# include <QProcess>
# include <QTemporaryFile>
# include "tbaidocument.h"
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

  static ReturnValue sign(TbaiDocument&);
  static ReturnValue sign(const TbaiCertificate&, TbaiDocument&);
};

#endif // TBAISIGNPROCESS_H
