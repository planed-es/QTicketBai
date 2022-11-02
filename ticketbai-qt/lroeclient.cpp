#include "lroeclient.h"
#include "qcompressor.h"
#include "tbaicertificate.h"
#include <QDebug>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSsl>
#include <qcurl.h>

static const QByteArray customHeaderPrefix  = "eus-bizkaia-n3-";
static const QByteArray xmlPrefix           = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
static const QByteArray productionHostname  = "https://sarrerak.bizkaia.eus";
static const QByteArray developmentHostname = "https://pruesarrerak.bizkaia.eus";
static bool              productionEnv() { return qgetenv("LROE_ENVIRONMENT") == "production"; }
static const QByteArray& lroeHostname()  { return productionEnv() ? productionHostname : developmentHostname; }

static void onFinished(QNetworkReply* reply, std::function<void()> callback)
{
  if (reply->isFinished())
    callback();
  else
    QObject::connect(reply, &QNetworkReply::finished, callback);
}

LROEClient::LROEClient(const CompanyData& emitter, QObject* parent) : QObject(parent), emitter(emitter)
{
}

LROEClient::LROEClient(QObject* parent) : QObject(parent), emitter(CompanyData::self)
{
}

void LROEClient::submit(const LROEDocument& document, std::function<void(const Response&)> callback)
{
  QNetworkReply* httpResponse = sendDocument(document);

  onFinished(httpResponse, [this, callback, httpResponse]()
  {
    Response lroeResponse = parseResponse(httpResponse);

    if (lroeResponse.status != 200)
      qDebug() << "QNetworkReply: error:" << httpResponse->errorString();
    httpResponse->deleteLater();
    callback(lroeResponse);
  });
}

QNetworkReply* LROEClient::sendDocument(const LROEDocument& document)
{
  QNetworkRequest   request;
  QUrl              endpoint(lroeHostname() + "/N3B4000M/aurkezpena");
  QJsonDocument     jsonHeader = jsonHeaderFor(document);
  QCurl             curl;
  QByteArray        xml = xmlPrefix + document.toString().toUtf8();
  QByteArray        compressedData;

  QCompressor::gzipCompress(xml, compressedData);
  request.setUrl(endpoint);
  request.setHeader(QNetworkRequest::ContentTypeHeader,   "application/octet-stream"); // Yes, technically incorrect. It's what the API expects.
  request.setHeader(QNetworkRequest::ContentLengthHeader, compressedData.length());
  request.setRawHeader("Content-Encoding",  "gzip");
  request.setRawHeader("Accepted-Encoding", "gzip");
  request.setRawHeader(customHeaderPrefix + "version",      LROEDocument::apiVersion);
  request.setRawHeader(customHeaderPrefix + "content-type", "application/xml");
  request.setRawHeader(customHeaderPrefix + "data",         jsonHeader.toJson(QJsonDocument::Compact));
  curl.setCertificate(TbaiCertificate::pemCertificatePath(), QSsl::Pem);
  curl.setSslKey(TbaiCertificate::pemKeyPath(), QSsl::Rsa);
  curl.setVerbosityLevel(7);
  return curl.send(request, compressedData);
}

QJsonDocument LROEClient::jsonHeaderFor(const LROEDocument& document)
{
  QJsonObject jsonHeader, jsonEmitter, jsonDrs;
  QByteArray  cif       = emitter.cif.toUtf8();
  QByteArray  name      = emitter.name.toUtf8();
  QByteArray  firstname = qgetenv("EUS_BIZKAIA_FIRSTNAME");
  QByteArray  lastname  = qgetenv("EUS_BIZKAIA_LASTNAME");

  jsonDrs.insert("mode", QString::number(static_cast<int>(document.modelType())));
  jsonDrs.insert("ejer", QString::number(document.activityYear()));
  jsonEmitter.insert("nif", QJsonValue::fromVariant(cif));
  jsonEmitter.insert("nrs", QJsonValue::fromVariant(name));
  if (!firstname.isEmpty())
    jsonEmitter.insert("ap1", QJsonValue::fromVariant(firstname));
  if (!lastname.isEmpty())
    jsonEmitter.insert("ap2", QJsonValue::fromVariant(lastname));
  jsonHeader.insert("con", "LROE"); // Concepto
  jsonHeader.insert("apa", document.documentTypeString()); // Apartado
  jsonHeader.insert("inte", jsonEmitter); // Interesado
  jsonHeader.insert("drs",  jsonDrs); // Datos relevantes
  return QJsonDocument(jsonHeader);
}

LROEClient::Response LROEClient::parseResponse(QNetworkReply* reply)
{
  Response   data;

  data.status  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
  data.type    = reply->rawHeader(customHeaderPrefix + "tipo-respuesta");
  data.code    = reply->rawHeader(customHeaderPrefix + "codigo-respuesta");
  data.message = QString(reply->rawHeader(customHeaderPrefix + "mensaje-respuesta"));
  data.id      = reply->rawHeader(customHeaderPrefix + "identificativo");
  if (reply->header(QNetworkRequest::ContentTypeHeader).toString().startsWith("application/xml"))
  {
    QByteArray rawBody = reply->readAll();
    QByteArray body;
    bool success = true;

    if (reply->rawHeader("Content-Encoding") == "gzip")
      success = QCompressor::gzipDecompress(rawBody, body);
    else
      body = rawBody;
    if (!success)
      qDebug() << "/!\\ Could not process response body for response" << data;
    data.document.setContent(body);
  }
  return data;
}

QDebug operator<<(QDebug d, const LROEClient::Response& response)
{
  QByteArray description = "LROEClient::Response(status="
    + QByteArray::number(response.status)
    + ", type=" + response.type
    + ", code=" + response.code
    + ", id=" + response.id
    + ", message=" + response.message.toUtf8()
    + ')';
  return d << description;
}
