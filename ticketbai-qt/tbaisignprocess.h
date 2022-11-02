#ifndef  TBAISIGNPROCESS_H
# define TBAISIGNPROCESS_H

# include <QObject>
# include <QProcess>
# include <QTemporaryFile>
# include "tbaidocument.h"

class QProcess;
class QDomDocument;
class TbaiInvoiceInterface;

class TICKETBAIQT_EXPORT TbaiSignProcess
{
public:
  static bool checkSettings();

  struct ReturnValue
  {
    ReturnValue(QString param = QString()) : error(param) {}
    operator bool() const { return error.isNull(); }
    QString error;
    QByteArray xml;
  };

  static ReturnValue sign(TbaiDocument&);
};

#endif // TBAISIGNPROCESS_H
