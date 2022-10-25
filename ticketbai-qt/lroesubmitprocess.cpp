#include "lroesubmitprocess.h"
#include "tbaicertificate.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <qcurl.h>
#include "qcompressor.h"
#include "companydata.h"
#define MAX_INVOICES_PER_LROE 1000

const QByteArray LROESubmitProcess::customHeaderPrefix = "eus-bizkaia-n3-";
const QString    LROESubmitProcess::dumpPath = LROESubmitProcess::getDumpPath();
static const QByteArray productionHostname  = "https://sarrerak.bizkaia.eus";
static const QByteArray developmentHostname = "https://pruesarrerak.bizkaia.eus";

static bool productionEnv() { return qgetenv("LROE_ENVIRONMENT") == "production"; }

static const QByteArray& lroeHostname() { return productionEnv() ? productionHostname : developmentHostname; }
static QString getCompanyName() { return productionEnv() ? CompanyData::self.name : "TestCompany"; }
static QString getCif()         { return productionEnv() ? CompanyData::self.cif : "A99805061"; }

LROESubmitProcess::LROESubmitProcess(QObject *parent) : QObject(parent)
{
}

QString LROESubmitProcess::getDumpPath()
{
  QString path = qgetenv("TBAI_OUTPUT_PATH");

  return path.isEmpty() ? "lroe-pending" : path;
}

QStringList LROESubmitProcess::pendingFiles()
{
  return QDir(dumpPath).entryList(QStringList() << "*.xml");
}

void LROESubmitProcess::submitAll()
{
  submitIf([](const QString&) -> bool { return true; });
}

void LROESubmitProcess::submitIf(std::function<bool (const QString &)> condition)
{
  QStringList selectedPaths;
  const QStringList entryList = pendingFiles();

  for (const QString& entry : entryList)
  {
    if (condition(entry))
      selectedPaths << entry;
  }
  breakDownQueryFor(selectedPaths);
  scheduleNextQuery();
}

void LROESubmitProcess::scheduleNextQuery()
{
  if (groups.length())
  {
    QStringList tbaiFiles = groups.first();

    groups.removeFirst();
    makeQueryFor(tbaiFiles);
  }
  else
    emit finished();
}

static void onFinished(QNetworkReply* reply, std::function<void()> callback)
{
  if (reply->isFinished())
    callback();
  else
    QObject::connect(reply, &QNetworkReply::finished, callback);
}

static void backupDocumentForDebugPurposes(const LROEDocument& document)
{
  QFile tmpfile("/tmp/lroe-tmp.xml");
  if (tmpfile.open(QIODevice::WriteOnly))
    tmpfile.write(document.toByteArray());
  else
    qDebug() << "(!) Could not backup LROEDocument to /tmp/lroe-tmp.xml";
}

void LROESubmitProcess::makeQueryFor(const QStringList &tbaiFiles)
{
  LROEDocument   document(LROEDocument::Model240);
  QNetworkReply* reply;

  document.initializeIncomeWithInvoices();
  document.setActivityYear(2022);
  document.setOperationId("TEST01");
  for (const QString& path : tbaiFiles)
    document.appendInvoiceFromFile(dumpPath + '/' + path);
  backupDocumentForDebugPurposes(document);
  reply = sendDocument(document);
  onFinished(reply, [this, reply]()
  {
    LROEResponse data = parseResponse(reply);

    if (data.status == 200)
      onResponseReceived(data);
    else
    {
      qDebug() << "QNetworkReply error message:" << reply->errorString();
      onResponseReceived(data);
    }
    reply->deleteLater();
  });
}

void LROESubmitProcess::onResponseReceived(const LROEResponse& response)
{
  // TODO
  qDebug() << "LROE Remote Server responded with:";
  qDebug() << "  Status: " << response.status;
  qDebug() << "  Type:"    << response.type;
  qDebug() << "  Code:"    << response.code;
  qDebug() << "  Msg :"    << response.message;
  qDebug() << "  Id  :"    << response.id;
  qDebug() << "  Document:";
  qDebug() << response.document.toByteArray(2) << "\n";
  // END TODO
  scheduleNextQuery();
}

LROESubmitProcess::LROEResponse LROESubmitProcess::parseResponse(QNetworkReply* reply)
{
  LROEResponse data;
  QByteArray   body;

  data.status  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
  data.type    = reply->rawHeader(customHeaderPrefix + "tipo-respuesta");
  data.code    = reply->rawHeader(customHeaderPrefix + "codigo-respuesta");
  data.message = QString::fromUtf8(reply->rawHeader(customHeaderPrefix + "mensaje-respuesta"));
  data.id      = reply->rawHeader(customHeaderPrefix + "identificativo");
  if (reply->header(QNetworkRequest::ContentTypeHeader) == "application/xml") // according to the documentation, these dimwits might've misspelled application/xml as application/XML.
  {
    QByteArray rawBody = reply->readAll();

    if (reply->rawHeader("Content-Encoding") == "gzip")
      QCompressor::gzipDecompress(reply->readAll(), body);
    else
      body = rawBody;
    data.document.setContent(body);
  }
  return data;
}

QJsonDocument LROESubmitProcess::jsonHeaderFor(const LROEDocument& document)
{
  QJsonObject jsonHeader, jsonEmitter, jsonDrs;
  QByteArray  cif       = getCif().toUtf8();
  QByteArray  name      = getCompanyName().toUtf8();
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

QNetworkReply* LROESubmitProcess::sendDocument(const LROEDocument& document)
{
  QNetworkRequest   request;
  QSslConfiguration sslConfiguration;
  QUrl              endpoint(lroeHostname() + "/N3B4000M/aurkezpena");
  QJsonDocument     jsonHeader = jsonHeaderFor(document);
  QByteArray        compressedData;
  QCurl             curl;

  QCompressor::gzipCompress(document.toByteArray(), compressedData);

  QFile file("Ejemplo_1_LROE_PF_140_IngresosConFacturaConSG_79732487C.xml.gz");
  if (file.open(QIODevice::ReadOnly))
    compressedData = file.readAll();

  request.setUrl(endpoint);
  request.setHeader(QNetworkRequest::ContentTypeHeader,   "application/octet-stream"); // Yes, technically incorrect. It's what the API expects.
  request.setHeader(QNetworkRequest::ContentLengthHeader, compressedData.length());
  request.setRawHeader("Content-Encoding",  "gzip");
  request.setRawHeader("Accepted-Encoding", "gzip");
  request.setRawHeader(customHeaderPrefix + "version",      LROEDocument::apiVersion);
  request.setRawHeader(customHeaderPrefix + "content-type", "application/xml");
  request.setRawHeader(customHeaderPrefix + "data",         jsonHeader.toJson(QJsonDocument::Compact));
  curl.setCertificate(TbaiCertificate::pemPath(), QSsl::Pem);
  curl.setSslKey(TbaiCertificate::keyPath(), QSsl::Rsa);
  curl.setVerbosityLevel(7);
  return curl.send(request, compressedData);
}

void LROESubmitProcess::breakDownQueryFor(const QStringList& tbaiFiles)
{
  groups.reserve(tbaiFiles.size() / MAX_INVOICES_PER_LROE + 1);
  for (int i = 0 ; i < tbaiFiles.size() ; ++i)
  {
    if (i % MAX_INVOICES_PER_LROE == 0)
    {
      groups.push_back(QStringList());
      groups.last().reserve(MAX_INVOICES_PER_LROE);
    }
    groups.last().push_back(tbaiFiles[i]);
  }
}
