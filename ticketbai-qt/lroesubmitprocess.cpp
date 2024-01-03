#include "lroesubmitprocess.h"
#include "tbaicertificate.h"
#include "uploaddocument.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include "companydata.h"
#define MAX_INVOICES_PER_LROE 1000

LROESubmitProcess::LROESubmitProcess(const TbaiContext& context, QObject *parent) : LROEClient(context, parent)
{
}

LROESubmitProcess::LROESubmitProcess(QObject *parent) : LROEClient(parent)
{
}

QString LROESubmitProcess::dumpPathOrFallback() const
{
  QString path = context.dumpPath();

  return path.isEmpty() ? "lroe-pending" : path;
}

QStringList LROESubmitProcess::pendingFiles() const
{
  return QDir(dumpPathOrFallback()).entryList(QStringList() << "*.xml");
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
    submittingFiles = groups.first();
    groups.removeFirst();
    makeQueryFor(submittingFiles);
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
  LROEUploadDocument document(LROEDocument::Model240, LROEDocument::AddOperation);
  QNetworkReply* reply;

  document.setActivityYear(2023);
  for (const QString& path : tbaiFiles)
    document.appendInvoiceFromFile(dumpPathOrFallback() + '/' + path);
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
  if (response.status == 200 && response.type != "Incorrecto")
  {
    qDebug() << "LROESubmitProcess: successfully submit";
    cleanupSubmittedFiles();
    scheduleNextQuery();
  }
  else
  {
    qDebug() << "LROESubmitProcess: failed to submit";
    emit finished();
  }
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

void LROESubmitProcess::cleanupSubmittedFiles()
{
  for (const QString& filename : submittingFiles)
  {
    QString filepath = dumpPathOrFallback() + '/' + filename;

    if (QFile(filepath).remove())
      qDebug() << "- remove submitted invoice" << filename;
    else
      qDebug() << "- failed to remove submitted invoice" << filepath;
  }
  submittingFiles.clear();
}
