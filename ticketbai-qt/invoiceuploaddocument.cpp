#include "invoiceuploaddocument.h"
#include <QFile>

InvoiceUploadDocument::InvoiceUploadDocument(LROEDocument::ModelType a, LROEDocument::OperationType b) : LROEDocument(a, b)
{
  setDocumentType(1, 1);
  incomeListEl = createElement("FacturasEmitidas");
  root.appendChild(incomeListEl);
}

void InvoiceUploadDocument::appendInvoiceFromFile(const QString& filepath)
{
  QFile file(filepath);

  if (file.open(QIODevice::ReadOnly))
    appendInvoice(file.readAll());
  else
    throw std::runtime_error("LROEDocument::appendInvoiceFromFile: cannot open " + filepath.toStdString());
}

void InvoiceUploadDocument::appendInvoice(const QString& invoiceXml)
{
  QDomElement invoiceEl = createElement("FacturaEmitida");
  QDomElement tbaiEl    = createElement("TicketBai");
  const QByteArray header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

  tbaiEl.appendChild(createTextNode((header + invoiceXml).toUtf8().toBase64()));
  invoiceEl.appendChild(tbaiEl);
  incomeListEl.appendChild(invoiceEl);
}



