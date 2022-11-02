#ifndef  TBAI_QUERY_DOCUMENT_H
# define TBAI_QUERY_DOCUMENT_H

# include "lroedocument.h"
# include "companydata.h"

void resetElement(QDomElement& el);

class TICKETBAIQT_EXPORT TbaiQueryDocument : public LROEDocument
{
public:
  TbaiQueryDocument(ModelType model) : LROEDocument(model, LROEDocument::QueryOperation)
  {
    filter          = createElement("FiltroConsultaFacturasEmitidasConSG");
    invoiceHeaderEl = createElement("CabeceraFactura");
    seriesFilter    = createElement("SerieFactura");
    dateFilter      = createElement("FechaExpedicionFactura");
    pageEl          = createElement("NumPaginaConsulta");
    recipientEl     = createElement("Destinatario");
    filter.appendChild(invoiceHeaderEl);
  }

  void setSeriesFilter(const QString& series)
  {
    resetElement(seriesFilter);
    seriesFilter.appendChild(createTextNode(series));
    invoiceHeaderEl.appendChild(seriesFilter);
  }

  void setNumberFilter(const QString& number)
  {
    resetElement(numberFilter);
    numberFilter.appendChild(createTextNode(number));
    invoiceHeaderEl.appendChild(numberFilter);
  }

  void setDateFilter(const QDateTime& from, const QDateTime& to)
  {
    QDomElement fromEl = createElement("Desde");
    QDomElement toEl   = createElement("Hasta");
    const QString format = "dd-mm-YYYY";

    fromEl.appendChild(createTextNode(from.toString(format)));
    toEl.appendChild(createTextNode(to.toString(format)));
    resetElement(dateFilter);
    dateFilter.appendChild(fromEl);
    dateFilter.appendChild(toEl);
  }

  void setRecipient(const CompanyData& company)
  {
    QDomElement el = createElement("NIF");
    el.appendChild(createTextNode(company.cif));
    resetElement(recipientEl);
    recipientEl.appendChild(el);
    filter.appendChild(el);
  }

  void setPage(unsigned short page)
  {
    resetElement(pageEl);
    pageEl.appendChild(createTextNode(QString::number(page)));
    filter.appendChild(pageEl);
  }

private:
  void initializeFilters()
  {
    root.appendChild(filter);
  }

  QDomElement filter, invoiceHeaderEl;
  QDomElement seriesFilter, numberFilter, dateFilter;
  QDomElement pageEl;
  QDomElement recipientEl;
};

#endif
