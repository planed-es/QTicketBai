#ifndef  LROECLIENT_H
# define LROECLIENT_H

# include <QObject>
# include <QJsonDocument>
# include <QDomDocument>
# include <functional>
# include "lroedocument.h"
# include "companydata.h"

class QNetworkReply;

class LROEClient : public QObject
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

  explicit LROEClient(const CompanyData& emitter, QObject* parent = nullptr);
  explicit LROEClient(QObject* parent = nullptr);
  virtual ~LROEClient() {}

  void submit(const LROEDocument& document, std::function<void(const Response&)> callback);

protected:
  QNetworkReply* sendDocument(const LROEDocument&);
  QJsonDocument  jsonHeaderFor(const LROEDocument&);
  Response       parseResponse(QNetworkReply*);
  void           onResponseReceived(const Response&);

  CompanyData emitter;
};

QDebug operator<<(QDebug d, const LROEClient::Response&);

#endif
