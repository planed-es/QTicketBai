#include "tbaidocument.h"
#include "tbaiinvoiceinterface.h"
#include "qticketbai.h"
#include <QUrl>
#include <QFile>

#define TBAI_VERSION          "1.2"
#define TBAI_SOFTWARE_VERSION "1.0"

static const QMap<TbaiInvoiceInterface::VatRegime, QByteArray> vatRegimes = {
  {TbaiInvoiceInterface::DefaultRegime,                                                                          "01"},
  {TbaiInvoiceInterface::ExportationRegime,                                                                      "02"},
  {TbaiInvoiceInterface::UsedGoodsRegime,                                                                        "03"},
  {TbaiInvoiceInterface::GoldInvestmentRegime,                                                                   "04"},
  {TbaiInvoiceInterface::TravelAgencyRegime,                                                                     "05"},
  {TbaiInvoiceInterface::EntityGroupVatRegime,                                                                   "06"},
  {TbaiInvoiceInterface::FundCriteraRegime,                                                                      "07"},
  {TbaiInvoiceInterface::RelatedToIPSIOrIGICRegime,                                                              "08"},
  {TbaiInvoiceInterface::ProxyTravelAgencyRegime,                                                                "09"},
  {TbaiInvoiceInterface::CollectionsOnBehalfOfThirdPartiesOfProfessionalFeesOrRights,                            "10"},
  {TbaiInvoiceInterface::LeasingOperationsOfBusinessPremisesSubjectToWithholding,                                "11"},
  {TbaiInvoiceInterface::LeasingOperationsOfBusinessPremisesNotSubjectToWithholding,                             "12"},
  {TbaiInvoiceInterface::LeasingOperationsOfBusinessPremises,                                                    "13"},
  {TbaiInvoiceInterface::InvoiceWithVatPendingAccrualInWorkCertificationsWhoseRecipientIsAPublicAdministration,  "14"},
  {TbaiInvoiceInterface::InvoiceWithVatPendingAccrualInSuccessiveTractOperations,                                "15"},
  {TbaiInvoiceInterface::OperationsInEquivalenceSurcharge,                                                       "51"},
  {TbaiInvoiceInterface::OperationsInSimplifiedRegime,                                                           "52"},
  {TbaiInvoiceInterface::OperationsCarriedOutByEntitiesNotConsideredBusinesspersonOrProfessionalsForVatPurposes, "53"}
};

static const QMap<TbaiInvoiceInterface::VatExemption, QByteArray> vatExemptionCodes = {
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle20,      "E1"},
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle21,      "E2"},
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle22,      "E3"},
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle23and24, "E4"},
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle25,      "E5"},
  {TbaiInvoiceInterface::VatExempted,                           "E6"}
};

TbaiDocument::TbaiDocument()
{

}

QDomElement generateContactXml(QDomDocument& document, const CompanyData& contact);

static QDomElement generateRecipientXml(QDomDocument& document, const CompanyData& recipient)
{
  QDomElement recipientEl = document.createElement("IDDestinatario");
  QDomElement nameEl      = document.createElement("ApellidosNombreRazonSocial");
  QDomElement postalCodeEl= document.createElement("CodigoPostal");

  recipientEl.appendChild(generateContactXml(document, recipient));
  nameEl.appendChild(document.createTextNode(recipient.name));
  recipientEl.appendChild(nameEl);
  if (recipient.postalCode.length() > 0)
  {
    postalCodeEl.appendChild(document.createTextNode(recipient.postalCode));
    recipientEl.appendChild(postalCodeEl);
  }
  return recipientEl;
}

static QDomElement generateRecipientXml(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root = document.createElement("Destinatarios");

  for (const CompanyData& recipient : invoice.recipients())
    root.appendChild(generateRecipientXml(document, recipient));
  return root;
}

static QByteArray getCodeForInvoiceType(TbaiInvoiceInterface::Type type)
{
  switch (type)
  {
    case TbaiInvoiceInterface::CorrectionArt80_1_Type:
    case TbaiInvoiceInterface::CorrectionArt80_2_Type:
    case TbaiInvoiceInterface::CorrectionArt80_6_Type:
      return "R1";
    case TbaiInvoiceInterface::CorrectionArt80_3_Type:
      return "R2";
    case TbaiInvoiceInterface::CorrectionArt80_4_Type:
      return "R3";
    case TbaiInvoiceInterface::CorrectionOthers_Type:
      return "R4";
    case TbaiInvoiceInterface::SimplifiedCorrectionType:
      return "R5";
  }
  return QByteArray();
}

static QDomElement generateCorrectionInvoiceHeader(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root   = document.createElement("FacturaRectificativa");
  QDomElement codeEl = document.createElement("Codigo");
  QDomElement typeEl = document.createElement("Tipo");

  codeEl.appendChild(document.createTextNode(getCodeForInvoiceType(invoice.invoiceType())));
  typeEl.appendChild(document.createTextNode("S")); // S for Substitution, I for difference
  // Missing optional element(s): ImporteRectificacion and Sustitutiva, or ImporteRectificacionSustitutiva
  root.appendChild(codeEl);
  root.appendChild(typeEl);
  return root;
}

static QDomElement generateInvoiceSubstitutionId(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root     = document.createElement("IDFacturaRectificadaSustituida");
  QDomElement seriesEl = document.createElement("SerieFactura");
  QDomElement numberEl = document.createElement("NumFactura");
  QDomElement dateEl   = document.createElement("FechaExpedicionFactura");

  if (invoice.series().length() > 0)
  {
    seriesEl.appendChild(document.createTextNode(invoice.series()));
    root.appendChild(seriesEl);
  }
  numberEl.appendChild(document.createTextNode(invoice.number()));
  dateEl.appendChild(document.createTextNode(invoice.date().toString("dd-MM-yyyy")));
  root.appendChild(numberEl);
  root.appendChild(dateEl);
  return root;
}

static QDomElement generateCorrectedInvoiceListHeader(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root = document.createElement("FacturasRectificadasSustituidas");

  for (const auto* correctedInvoice : invoice.correctedInvoices())
    root.appendChild(generateInvoiceSubstitutionId(document, *correctedInvoice));
  return root;
}

static QDomElement generateInvoiceHeader(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root     = document.createElement("CabeceraFactura");
  QDomElement serieEl  = document.createElement("SerieFactura");
  QDomElement numberEl = document.createElement("NumFactura");
  QDomElement dateEl   = document.createElement("FechaExpedicionFactura");
  QDomElement hourEl   = document.createElement("HoraExpedicionFactura");

  serieEl .appendChild(document.createTextNode(invoice.series()));
  numberEl.appendChild(document.createTextNode(invoice.number()));
  dateEl  .appendChild(document.createTextNode(invoice.date().toString("dd-MM-yyyy")));
  hourEl  .appendChild(document.createTextNode(invoice.date().toString("hh:mm:ss")));
  if (invoice.series().length() > 0)
    root.appendChild(serieEl);
  root.appendChild(numberEl);
  root.appendChild(dateEl);
  root.appendChild(hourEl);
  if (invoice.invoiceType() != TbaiInvoiceInterface::InvoiceType)
  {
    if (invoice.series().isEmpty())
    {
      qDebug() << "Failed to generate TbaiDocument for invoice " << invoice.number() << ": series cannot be empty for rectificative invoices";
      throw std::runtime_error("QTicketBai:TbaiDocument: invoice.series() cannot be empty for rectificative invoices.");
    }
    root.appendChild(generateCorrectionInvoiceHeader(document, invoice));
    if (invoice.correctedInvoices().size() > 0)
      root.appendChild(generateCorrectedInvoiceListHeader(document, invoice));
  }
  return root;
}

static QDomElement generateInvoiceData(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root          = document.createElement("DatosFactura");
  QDomElement descriptionEl = document.createElement("DescripcionFactura");
  QDomElement amountEl      = document.createElement("ImporteTotalFactura");
  QDomElement keysEl        = document.createElement("Claves");

  if (invoice.description().length() == 0)
    throw std::runtime_error("QTicketBai::TbaiDocument: invoice.description() cannot be empty");
  descriptionEl.appendChild(document.createTextNode(invoice.description().toUtf8()));
  amountEl.appendChild(document.createTextNode(invoice.formattedAmount()));
  for (auto vatRegime : invoice.vatRegimes())
  {
    QDomElement keyEl       = document.createElement("IDClave");
    QDomElement vatRegimeEl = document.createElement("ClaveRegimenIvaOpTrascendencia");

    vatRegimeEl.appendChild(document.createTextNode(vatRegimes[vatRegime]));
    keyEl.appendChild(vatRegimeEl);
    keysEl.appendChild(keyEl);
  }
  root.appendChild(descriptionEl);
  root.appendChild(amountEl);
  root.appendChild(keysEl);
  return root;
}

static const QMap<TbaiInvoiceInterface::NotSubjectToVatReason, QString> notSubjectToVatReasons = {
  {TbaiInvoiceInterface::VatExemptedByNormaForalArticle7, "OT"},
  {TbaiInvoiceInterface::VatExemptedDueToLocalization,    "RL"}
};

static const QMap<TbaiInvoiceInterface::NonExemptedVatType, QByteArray> nonExemptedVatTypes = {
  {TbaiInvoiceInterface::WithInversionOfPassiveSubject,    "S2"},
  {TbaiInvoiceInterface::WithoutInversionOfPassiveSubject, "S1"}
};

static QDomElement generateVatDetailsForNonSubjectToVat(QDomDocument& document, const TbaiInvoiceInterface::VatBreakdown& breakdown)
{
  QDomElement detailEl = document.createElement("DetalleNoSujeta");
  QDomElement reasonEl = document.createElement("Causa");
  QDomElement amountEl = document.createElement("Importe");

  reasonEl.appendChild(document.createTextNode(notSubjectToVatReasons[breakdown.vatState]));
  amountEl.appendChild(document.createTextNode(QString::number(breakdown.total(), 'f', 2)));
  detailEl.appendChild(reasonEl);
  detailEl.appendChild(amountEl);
  return detailEl;
}

static QDomElement generateVatDetailsForExemptedToVat(QDomDocument& document, const TbaiInvoiceInterface::VatBreakdown& breakdown)
{
  QDomElement detailEl = document.createElement("DetalleExenta");
  QDomElement reasonEl = document.createElement("CausaExencion");
  QDomElement amountEl = document.createElement("BaseImponible");

  reasonEl.appendChild(document.createTextNode(vatExemptionCodes[breakdown.exemptionType]));
  amountEl.appendChild(document.createTextNode(QString::number(breakdown.total(), 'f', 2)));
  detailEl.appendChild(reasonEl);
  detailEl.appendChild(amountEl);
  return detailEl;
}

static QDomElement generateVatDetails(QDomDocument& document, const TbaiInvoiceInterface::VatBreakdown& breakdown)
{
  QDomElement detailEl      = document.createElement("DetalleIVA");
  QDomElement baseEl        = document.createElement("BaseImponible");
  QDomElement taxRateEl     = document.createElement("TipoImpositivo");
  QDomElement taxFeeEl      = document.createElement("CuotaImpuesto");
  QDomElement recargoRateEl = document.createElement("TipoRecargoEquivalencia");
  QDomElement recargoFeeEl  = document.createElement("CuotaRecargoEquivalencia");
  QDomElement recargoModeEl = document.createElement("OperacionEnRecargoDeEquivalenciaORegimenSimplificado");

  baseEl       .appendChild(document.createTextNode(QString::number(breakdown.base, 'f', 2)));
  taxRateEl    .appendChild(document.createTextNode(QString::number(breakdown.taxRate * 100, 'f', 2)));
  taxFeeEl     .appendChild(document.createTextNode(QString::number(breakdown.taxFee(), 'f', 2)));
  recargoRateEl.appendChild(document.createTextNode(QString::number(breakdown.recargoRate * 100, 'f', 2)));
  recargoFeeEl .appendChild(document.createTextNode(QString::number(breakdown.recargoFee(), 'f', 2)));
  recargoModeEl.appendChild(document.createTextNode(breakdown.recargoSimplifiedRegime ? "Y" : "N"));
  detailEl.appendChild(baseEl);
  detailEl.appendChild(taxRateEl);
  detailEl.appendChild(taxFeeEl);
  detailEl.appendChild(recargoRateEl);
  detailEl.appendChild(recargoFeeEl);
  detailEl.appendChild(recargoModeEl);
  return detailEl;
}

static QList<TbaiInvoiceInterface::VatBreakdown> breakdownWithVat(const TbaiInvoiceInterface& invoice)
{
  QList<TbaiInvoiceInterface::VatBreakdown> result;

  for (const TbaiInvoiceInterface::VatBreakdown& breakdown : invoice.vatBreakdowns())
  {
    if (breakdown.vatState == TbaiInvoiceInterface::SubjectToVat && breakdown.exemptionType == TbaiInvoiceInterface::NoVatExemption)
      result << breakdown;
  }
  return result;
}

static QList<TbaiInvoiceInterface::VatBreakdown> breakdownNotSubjectToVat(const TbaiInvoiceInterface& invoice)
{
  QList<TbaiInvoiceInterface::VatBreakdown> result;

  for (const TbaiInvoiceInterface::VatBreakdown& breakdown : invoice.vatBreakdowns())
  {
    if (breakdown.vatState != TbaiInvoiceInterface::SubjectToVat)
      result << breakdown;
  }
  return result;
}

static QList<TbaiInvoiceInterface::VatBreakdown> breakdownWithExemptedVat(const TbaiInvoiceInterface& invoice)
{
  QList<TbaiInvoiceInterface::VatBreakdown> result;

  for (const TbaiInvoiceInterface::VatBreakdown& breakdown : invoice.vatBreakdowns())
  {
    if (breakdown.vatState == TbaiInvoiceInterface::SubjectToVat && breakdown.exemptionType != TbaiInvoiceInterface::NoVatExemption)
      result << breakdown;
  }
  return result;
}

static QDomElement generateInvoiceBreakdown(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root = document.createElement("TipoDesglose");
  QDomElement wrapper = document.createElement("DesgloseFactura");

  if (breakdownNotSubjectToVat(invoice).size())
  {
    QDomElement notSubjectEl = document.createElement("NoSujeta");

    for (const TbaiInvoiceInterface::VatBreakdown& breakdown : breakdownNotSubjectToVat(invoice))
      notSubjectEl.appendChild(generateVatDetailsForNonSubjectToVat(document, breakdown));
    wrapper.appendChild(notSubjectEl);
  }
  if (breakdownWithVat(invoice).size() > 0 || breakdownWithExemptedVat(invoice).size() > 0)
  {
    QDomElement subjectEl = document.createElement("Sujeta");

    if (breakdownWithVat(invoice).size())
    {
      QDomElement notExemptedEl = document.createElement("NoExenta");

      for (auto it = nonExemptedVatTypes.begin() ; it != nonExemptedVatTypes.end() ; ++it)
      {
        QDomElement breakdownRoot  = document.createElement("DetalleNoExenta");
        QDomElement typeEl         = document.createElement("TipoNoExenta");
        QDomElement breakdownVatEl = document.createElement("DesgloseIVA");

        typeEl.appendChild(document.createTextNode(it.value()));
        for (const TbaiInvoiceInterface::VatBreakdown& breakdown : breakdownWithVat(invoice))
        {
          if (breakdown.nonExemptedType == it.key())
            breakdownVatEl.appendChild(generateVatDetails(document, breakdown));
        }
        if (!breakdownVatEl.lastChild().isNull())
        {
          breakdownRoot.appendChild(typeEl);
          breakdownRoot.appendChild(breakdownVatEl);
          notExemptedEl.appendChild(breakdownRoot);
        }
      }
      subjectEl.appendChild(notExemptedEl);
    }
    if (breakdownWithExemptedVat(invoice).size())
    {
      QDomElement exemptedEl = document.createElement("Exenta");

      for (const TbaiInvoiceInterface::VatBreakdown& breakdown : breakdownWithExemptedVat(invoice))
        exemptedEl.appendChild(generateVatDetailsForExemptedToVat(document, breakdown));
      subjectEl.appendChild(exemptedEl);
    }
    wrapper.appendChild(subjectEl);
  }
  root.appendChild(wrapper);
  return root;
}

static QDomElement pluralSubjectHintXml(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement element = document.createElement("VariosDestinatarios");
  QString text = invoice.recipients().size() > 1 ? "Y" : "N";

  element.appendChild(document.createTextNode(text));
  return element;
}

TbaiDocument& TbaiDocument::createFrom(const TbaiInvoiceInterface& invoice)
{
  QDomElement subjectsEl      = createElement("Sujetos");
  QDomElement invoiceEl       = createElement("Factura");
  QDomElement footPrintEl     = createElement("HuellaTBAI");

  prepareDocument();
  subjectsEl.appendChild(generateEmitter());
  if (invoice.recipients().size() > 0)
    subjectsEl.appendChild(generateRecipientXml(*this, invoice));
  if (invoice.recipients().size() > 1)
    subjectsEl.appendChild(pluralSubjectHintXml(*this, invoice));
  root.appendChild(subjectsEl);
  invoiceEl.appendChild(generateInvoiceHeader(*this, invoice));
  invoiceEl.appendChild(generateInvoiceData(*this, invoice));
  invoiceEl.appendChild(generateInvoiceBreakdown(*this, invoice));
  root.appendChild(invoiceEl);
  root.appendChild(generateFingerprint(invoice));
  return *this;
}

static QString getDefaultFileNameFor(const TbaiInvoiceInterface& invoice)
{
  if (invoice.recipients().size() > 0)
  {
    const CompanyData& contact = invoice.recipients().first();

    return QUrl::toPercentEncoding(contact.id + '-' + contact.name, " ");
  }
  return "unnamed";
}

QString TbaiDocument::getFileNameFor(const TbaiInvoiceInterface& invoice)
{
  QString time    = invoice.date().toString("yyyyMMddhhmmss");
  QString title   = invoice.name();

  if (title.isEmpty())
    title = getDefaultFileNameFor(invoice);
  else
    title = QUrl::toPercentEncoding(title, " ");
  return time + '_' + title + ".xml";
}
