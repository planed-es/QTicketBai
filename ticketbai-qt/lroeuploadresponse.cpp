#include "lroeuploadresponse.h"

static QString extractStringValue(const QDomElement root, const QString& tagName)
{
  auto candidates = root.elementsByTagName(tagName);
  if (candidates.size() > 0)
    return candidates.at(0).toElement().text();
  return QString();
}

static QDate extractDateValue(const QDomElement root, const QString& tagName)
{
  QString text = extractStringValue(root, tagName);
  return QDate::fromString(text, "dd-MM-yyyy");
}

LROEUploadResponseDocument::LROEUploadResponseDocument(QDomDocument document)
{
  auto nodes = document.elementsByTagName("Registro"); 

  for (int i = 0 ; i < nodes.size() ; ++i)
    addEntry(nodes.at(i).toElement());
}

LROEUploadResponseDocument::Entry LROEUploadResponseDocument::fromTicketBaiXml(const QString& xml)
{
  TbaiDocument document;

  document.loadFrom(xml);
  return {
    document.invoiceNumber(),
    document.invoiceSerie(),
    document.invoiceDate()
  };
}

LROEUploadResponseDocument::Entry LROEUploadResponseDocument::fromIdentificator(const QDomElement identificator)
{
  auto ticketbai = identificator.elementsByTagName("TicketBai").at(0);
    
  if (ticketbai.isNull())
  {
    return {
      extractStringValue(identificator, "NumFactura"),
      extractStringValue(identificator, "SerieFactura"),
      extractDateValue(identificator, "FechaExpedicionFactura"),
    };
  }
  return fromTicketBaiXml(
    QByteArray::fromBase64(
      ticketbai.toElement().text().toUtf8()
    )
  );
}

void LROEUploadResponseDocument::addEntry(const QDomElement element)
{
  QDomNode identificator = element.elementsByTagName("Identificador").at(0);
  Entry entry;

  if (!identificator.isNull())
    entry = fromIdentificator(identificator.toElement());
  entry.success = extractStringValue(element, "EstadoRegistro") == "Correcto";
  if (!entry.success)
  {
    entry.errorCode    = extractStringValue(element, "CodigoErrorRegistro").toUtf8();
    entry.errorMessage = extractStringValue(element, "DescripcionErrorRegistroES").toUtf8();
  }
  m_entries << entry;
}

std::optional<LROEUploadResponseDocument::Entry> LROEUploadResponseDocument::find(const TbaiInvoiceInterface& invoice) const
{
  auto it = std::find(m_entries.begin(), m_entries.end(), invoice);

  return it != m_entries.end() ? std::optional<Entry>{*it} : std::optional<Entry>{};
}

std::optional<LROEUploadResponseDocument::Entry> LROEUploadResponseDocument::find(const QByteArray& series, const QByteArray& number) const
{
  auto it = std::find_if(m_entries.begin(), m_entries.end(), [&series, &number](const Entry& entry)
  {
    return entry.number == number && entry.series == series;
  });

  return it != m_entries.end() ? std::optional<Entry>{*it} : std::optional<Entry>{};
}

bool LROEUploadResponseDocument::passed(const TbaiInvoiceInterface& invoice) const
{
  auto entry = find(invoice);

  return entry && entry->success;
}

bool LROEUploadResponseDocument::passed(const QByteArray& series, const QByteArray& number) const
{
  auto entry = find(series, number);

  return entry && entry->success;
}

static const TbaiInvoiceInterface* findNextInvoice(const QList<TbaiInvoiceInterface*>& list, const TbaiInvoiceInterface* invoice)
{
  for (const auto* candidate : list)
  {
    TbaiInvoiceInterface* previousInvoice = candidate->previousInvoice();

    if (previousInvoice
    && previousInvoice->number() == invoice->number()
    && previousInvoice->series() == invoice->series())
      return candidate;
  }
  return nullptr;
}

static bool isNextInvoice(const TbaiInvoiceInterface* a, const TbaiInvoiceInterface* b, const QList<TbaiInvoiceInterface*>& list)
{
  const TbaiInvoiceInterface* result;

  while ((result = findNextInvoice(list, a)))
  {
    if (result == b)
      return true;
  }
  return false;
}

static QSharedPointer<TbaiDocument> findNextInvoice(const QList<QSharedPointer<TbaiDocument>>& list, const QSharedPointer<TbaiDocument>& invoice)
{
  for (const auto& candidate : list)
  {
    if (candidate->previousInvoiceNumber() == invoice->invoiceNumber()
    && candidate->previousInvoiceSerie() == invoice->invoiceSerie())
      return candidate;
  }
  return nullptr;
}

static bool isNextDocument(QSharedPointer<TbaiDocument> a, QSharedPointer<TbaiDocument> b, const QList<QSharedPointer<TbaiDocument>>& list)
{
  QSharedPointer<TbaiDocument> result = a;

  while ((result = findNextInvoice(list, result)))
  {
    if (result == b)
      return true;
  }
  return false;
}

QStringList LROEUploadResponseDocument::submittedFiles(const QStringList& filepathes) const
{
  QStringList submitted;
  QList<QSharedPointer<TbaiDocument>> documents;
  QMap<TbaiDocument*, QString> pathMap;

  for (const QString& filepath : filepathes)
  {
    QSharedPointer<TbaiDocument> document(new TbaiDocument);

    document->loadFromFile(filepath);
    documents << document;
    pathMap[document.get()] = filepath;
  }
  std::sort(
    documents.begin(),
    documents.end(),
    std::bind(isNextDocument, std::placeholders::_1, std::placeholders::_2, std::ref(documents))
  );
  for (const auto& document : documents)
  {
    if (passed(document->invoiceSerie(), document->invoiceNumber()))
      submitted << pathMap[document.get()];
    else
      break ;
  }
  return submitted;
}

QStringList LROEUploadResponseDocument::submittedFiles(QList<TbaiInvoiceInterface*> invoices) const
{
  QStringList result;

  std::sort(
    invoices.begin(),
    invoices.end(),
    std::bind(isNextInvoice, std::placeholders::_1, std::placeholders::_2, std::ref(invoices))
  );
  for (const auto* invoice : invoices)
  {
    if (passed(*invoice))
      result << TbaiDocument::getFileNameFor(*invoice);
    else
      break ;
  }
  return result;
}
