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
  bool wait();

signals:
  void generatedSignature(QByteArray);
  void generatedXml(QByteArray);
  void failed(QString error);
  void finished();

private slots:
  void onAutofirmaDone(int exitStatus, QProcess::ExitStatus);
  void onAutofirmaFailed(QProcess::ProcessError);

private:
  void launchAutofirma();
  bool findSignatureIn(const QDomDocument&);
  void generateXml(const QDomDocument& xmlSignature);

  TbaiDocument   document;
  QProcess*      autofirma;
  QTemporaryFile inputFile, outputFile;
};

#endif // TBAISIGNPROCESS_H
