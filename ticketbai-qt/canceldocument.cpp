#include "canceldocument.h"
#include "tbaiinvoiceinterface.h"

LROECancelDocument::LROECancelDocument(LROEDocument::ModelType model) : LROEUploadDocument(model, CancelOperation)
{
  setDocumentType(1, 1);
}

TbaiCancelDocument::TbaiCancelDocument()
{
}

static QDomElement generateInvoiceHeader(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root     = document.createElement("CabeceraFactura");
  QDomElement serieEl  = document.createElement("SerieFactura");
  QDomElement numberEl = document.createElement("NumFactura");
  QDomElement dateEl   = document.createElement("FechaExpedicionFactura");

  serieEl .appendChild(document.createTextNode(invoice.series()));
  numberEl.appendChild(document.createTextNode(invoice.number()));
  dateEl  .appendChild(document.createTextNode(invoice.date().toString("dd-MM-yyyy")));
  if (invoice.series().length() > 0)
    root.appendChild(serieEl);
  root.appendChild(numberEl);
  root.appendChild(dateEl);
  return root;
}

TbaiCancelDocument& TbaiCancelDocument::createFrom(const TbaiInvoiceInterface& invoice)
{
  QDomElement invoiceEl = createElement("IDFactura");

  prepareDocument();
  invoiceEl.appendChild(generateEmitter());
  invoiceEl.appendChild(generateInvoiceHeader(*this, invoice));
  root.appendChild(invoiceEl);
  root.appendChild(generateFingerprint(invoice));
  return *this;
}
