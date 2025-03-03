#pragma once
#include "tbaiinvoiceinterface.h"
#include "tbaidocument.h"

class LROEUploadResponseDocument
{
public:
  struct Entry
  {
    QString    number;
    QString    series;
    QDate      date;
    bool       success;
    QByteArray errorCode;
    QString    errorMessage;

    bool operator==(const TbaiInvoiceInterface& invoice) const { return invoice.series() == series && invoice.number() == number; }
    bool isValid() const { return !number.isEmpty(); }
  };

  LROEUploadResponseDocument(QDomDocument document);

  const QList<Entry>& entries() const { return m_entries; }
  std::optional<Entry> find(const TbaiInvoiceInterface&) const;
  std::optional<Entry> find(const QByteArray& series, const QByteArray& number) const;
  bool passed(const TbaiInvoiceInterface&) const;
  bool passed(const QByteArray& series, const QByteArray& number) const;
  QStringList submittedFiles(const QStringList& filenames) const;
  QStringList submittedFiles(QList<TbaiInvoiceInterface*>) const;

private:
  void addEntry(const QDomElement element);
  Entry fromIdentificator(const QDomElement identificator);
  Entry fromTicketBaiXml(const QString& xml);

  QList<Entry> m_entries;
};
