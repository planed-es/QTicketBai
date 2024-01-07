#ifndef  LROECLIENT_H
# define LROECLIENT_H

# include <QObject>
# include <QJsonDocument>
# include <QDomDocument>
# include <functional>
# include "lroedocument.h"
# include "context.h"

class QNetworkReply;

class TICKETBAIQT_EXPORT LROEClient : public QObject
{
  Q_OBJECT
public:
  struct Response
  {
    unsigned int status;
    QByteArray   id, type, code;
    QString      message;
    QDomDocument document;
  };

  typedef std::function<void (const Response&)> Callback;

  explicit LROEClient(const TbaiContext&, QObject* parent = nullptr);
  explicit LROEClient(QObject* parent = nullptr);
  virtual ~LROEClient() {}

  void submit(const LROEDocument& document, Callback callback = Callback());

signals:
  void responseReceived(Response);

protected:
  QNetworkReply* sendDocument(const LROEDocument&);
  QJsonDocument  jsonHeaderFor(const LROEDocument&);
  Response       parseResponse(QNetworkReply*);

  const TbaiContext& context;
};

QDebug operator<<(QDebug d, const LROEClient::Response&);

#endif
