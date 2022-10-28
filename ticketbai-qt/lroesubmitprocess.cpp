#include "lroesubmitprocess.h"
#include "tbaicertificate.h"
#include "invoiceuploaddocument.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <qcurl.h>
#include "qcompressor.h"
#include "companydata.h"
#define MAX_INVOICES_PER_LROE 1000

const QString    LROESubmitProcess::dumpPath = LROESubmitProcess::getDumpPath();

static bool productionEnv() { return qgetenv("LROE_ENVIRONMENT") == "production"; }

LROESubmitProcess::LROESubmitProcess(const CompanyData& emitter, QObject *parent) : LROEClient(emitter, parent)
{
}

LROESubmitProcess::LROESubmitProcess(QObject *parent) : LROEClient(parent)
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
  InvoiceUploadDocument document(LROEDocument::Model240, LROEDocument::AddOperation);
  QNetworkReply* reply;

  document.setActivityYear(2022);
  document.setOperationId("TEST01");
  for (const QString& path : tbaiFiles)
    document.appendInvoiceFromFile(dumpPath + '/' + path);
  backupDocumentForDebugPurposes(document);
  submit(document, std::bind(&LROESubmitProcess::onResponseReceived, this, std::placeholders::_1));
}

void LROESubmitProcess::onResponseReceived(const Response& response)
{
  // TODO
  qDebug() << "LROE remote server responded with:" << response;
  qDebug() << "  Document:";
  qDebug() << response.document.toByteArray(2) << "\n";
  // END TODO
  if (response.status == 200)
    scheduleNextQuery();
  else
    emit finished();
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
