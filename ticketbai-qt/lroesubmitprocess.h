#ifndef  LROESUBMITPROCESS_H
# define LROESUBMITPROCESS_H

# include <QStringList>
# include <QVector>
# include <QNetworkReply>
# include "lroeclient.h"

class LROESubmitProcessTest;

class LROESubmitProcess : public LROEClient
{
  Q_OBJECT
  friend class LROESubmitProcessTest;
public:
  static const QString dumpPath;

  explicit LROESubmitProcess(const CompanyData& emitter, QObject* parent = nullptr);
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
  void           onResponseReceived(const Response&);

  QVector<QStringList> groups;
};

#endif // LROESUBMITPROCESS_H
