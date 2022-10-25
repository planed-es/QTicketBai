#ifndef  LROESUBMITPROCESS_H
# define LROESUBMITPROCESS_H

# include <QObject>
# include <QProcess>
# include <QTemporaryFile>
# include <QStringList>
# include <QVector>
# include <QNetworkReply>
# include "lroedocument.h"

class LROESubmitProcessTest;

class LROESubmitProcess : public QObject
{
  Q_OBJECT
  friend class LROESubmitProcessTest;
public:
  static const QString dumpPath;

  struct LROEResponse
  {
    unsigned int status;
    QByteArray   id, type, code;
    QString      message;
    QDomDocument document;
  };

  explicit LROESubmitProcess(QObject *parent = nullptr);

  static QString getDumpPath();
  static QStringList pendingFiles();

  void submitAll();
  void submitIf(std::function<bool (const QString&)> condition);

signals:
  void finished();

private:
  void           breakDownQueryFor(const QStringList& tbaiFiles);
  void           makeQueryFor(const QStringList& tbaiFiles);
  void           scheduleNextQuery();
  QNetworkReply* sendDocument(const LROEDocument&);
  QJsonDocument  jsonHeaderFor(const LROEDocument&);
  LROEResponse   parseResponse(QNetworkReply*);
  void           onResponseReceived(const LROEResponse&);

  static const QByteArray customHeaderPrefix;
  QVector<QStringList>    groups;
  QTemporaryFile          xmlFile;
  QProcess                gunzip;
};

#endif // LROESUBMITPROCESS_H
