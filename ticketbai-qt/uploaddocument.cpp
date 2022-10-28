#include "uploaddocument.h"
#include <QFile>

static QString listTagFor(LROEDocument::ModelType type)
{
  return type == LROEDocument::Model240 ? "FacturasEmitidas" : "Ingresos";
}

static QString itemTagFor(LROEDocument::ModelType type)
{
  return type == LROEDocument::Model240 ? "FacturaEmitida" : "Ingreso";
}

static QString tbaiTagFor(LROEDocument::OperationType type)
{
  return type == LROEDocument::CancelOperation ? "AnulacionTicketBai" : "TicketBai";
}

TbaiUploadDocument::TbaiUploadDocument(LROEDocument::ModelType a, LROEDocument::OperationType operation) : LROEDocument(a, operation)
{
  setDocumentType(1, 1);
  incomeListEl = createElement(listTagFor(model));
  root.appendChild(incomeListEl);
}

void TbaiUploadDocument::appendInvoiceFromFile(const QString& filepath)
{
  QFile file(filepath);

  if (file.open(QIODevice::ReadOnly))
    appendInvoice(file.readAll());
  else
    throw std::runtime_error("LROEDocument::appendInvoiceFromFile: cannot open " + filepath.toStdString());
}

void TbaiUploadDocument::appendInvoice(const QString& invoiceXml)
{
  QDomElement invoiceEl = createElement(itemTagFor(model));
  QDomElement tbaiEl    = createElement(tbaiTagFor(operation));
  const QByteArray header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

  tbaiEl.appendChild(createTextNode((header + invoiceXml).toUtf8().toBase64()));
  invoiceEl.appendChild(tbaiEl);
  incomeListEl.appendChild(invoiceEl);
  if (model == Model140 && operation == AddOperation)
    invoiceEl.appendChild(appendRevenue(invoiceXml));
}

QDomElement TbaiUploadDocument::appendRevenue(const QString& invoiceXml)
{
  QDomElement revenueEl  = createElement("Renta");
  QDomElement detailEl   = createElement("DetalleRenta");
  QDomElement epigraphEl = createElement("Epigrafe");

  epigraphEl.appendChild(createTextNode("197330"));
  detailEl.appendChild(epigraphEl);
  revenueEl.appendChild(detailEl);
  return revenueEl;
}
