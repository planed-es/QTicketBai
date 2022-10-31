#ifndef  TBAISIGNPROCESS_H
# define TBAISIGNPROCESS_H

# include <QObject>
# include <QProcess>
# include <QTemporaryFile>
# include "tbaidocument.h"

class QProcess;
class QDomDocument;
class TbaiInvoiceInterface;

class TbaiSignProcess : public QObject
{
  Q_OBJECT
public:
  explicit TbaiSignProcess(QObject *parent = nullptr);

  static bool checkSettings();

  void sign(const TbaiInvoiceInterface&);

signals:
  void generatedSignature(QByteArray);
  void generatedXml(QByteArray);
  void failed(QString error);
  void finished();

private:
  TbaiDocument   document;
};

#endif // TBAISIGNPROCESS_H
