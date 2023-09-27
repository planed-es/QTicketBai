#ifndef  LROESUBMITPROCESS_H
# define LROESUBMITPROCESS_H

# include <QStringList>
# include <QVector>
# include <QNetworkReply>
# include "lroeclient.h"

class LROESubmitProcessTest;

class TICKETBAIQT_EXPORT LROESubmitProcess : public LROEClient
{
  Q_OBJECT
  friend class LROESubmitProcessTest;
public:
  explicit LROESubmitProcess(const TbaiContext&, QObject* parent = nullptr);
  explicit LROESubmitProcess(QObject* parent = nullptr);

  QString dumpPathOrFallback() const;
  QStringList pendingFiles() const;

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
