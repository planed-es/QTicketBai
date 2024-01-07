#include "lroesubmitprocess.h"
#include "tbaicertificate.h"
#include "uploaddocument.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include "companydata.h"
#define MAX_INVOICES_PER_LROE 1000

static int getDocumentYear(const QString& filepath)
{
  QFile file(filepath);

  if (file.open(QIODevice::ReadOnly))
  {
    QDomDocument document;
    QDomNodeList list;

    document.setContent(&file);
    list = document.elementsByTagName("FechaExpedicionFactura");
    if (!list.isEmpty())
    {
      QString text = list.item(0).toElement().text();
      QDate date = QDate::fromString(text, "dd-MM-yyyy");

      if (date.year() == 0)
        throw std::runtime_error("Invalid date '" + text.toStdString() + "' in tbai document at " + filepath.toStdString());
      return date.year();
    }
    throw std::runtime_error("Broken tbai document at " + filepath.toStdString());
  }
  throw std::runtime_error("Cannot open tbai document at " + filepath.toStdString());
}

LROESubmitProcess::LROESubmitProcess(const TbaiContext& context, QObject *parent) : LROEClient(context, parent)
{
  connect(this, &LROEClient::responseReceived, this, &LROESubmitProcess::onResponseReceived);
}

LROESubmitProcess::LROESubmitProcess(QObject *parent) : LROEClient(parent)
{
  connect(this, &LROEClient::responseReceived, this, &LROESubmitProcess::onResponseReceived);
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

QString LROESubmitProcess::storagePathFromFileName(const QString& filename) const
{
  return dumpPathOrFallback() + '/' + filename;
}

void LROESubmitProcess::makeQueryFor(const QStringList &tbaiFiles)
{
  LROEUploadDocument document(LROEDocument::Model240, LROEDocument::AddOperation);
  int year = getDocumentYear(storagePathFromFileName(tbaiFiles.first()));

  for (const QString& path : tbaiFiles)
    document.appendInvoiceFromFile(storagePathFromFileName(path));
  document.setActivityYear(year);
  backupDocumentForDebugPurposes(document);
  submit(document);
}

void LROESubmitProcess::onResponseReceived(const Response& response)
{
  // TODO
  qDebug() << "LROE remote server responded with:" << response;
  qDebug() << "  Document:";
  qDebug() << qPrintable(response.document.toByteArray(2)) << "\n";
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
    QString filepath = storagePathFromFileName(filename);

    if (QFile(filepath).remove())
      qDebug() << "- remove submitted invoice" << filename;
    else
      qDebug() << "- failed to remove submitted invoice" << filepath;
  }
  submittingFiles.clear();
}
