#include "tbaidocument.h"
#include "invoiceinterface.h"
#include "companydata.h"
#include <QUrl>
#include <QFile>

#define TBAI_VERSION          "1.2"
#define TBAI_SOFTWARE_VERSION "1.0"

namespace VatPolicy
{
  static const QByteArray Default        = "01";
  static const QByteArray Exportation    = "02";
  static const QByteArray UsedGoods      = "03";
  static const QByteArray GoldInvestment = "04";
  static const QByteArray TravelAgency   = "05";
  static const QByteArray EntityGroupVat = "06";
  static const QByteArray FundCriteria   = "07";
  static const QByteArray RelatedToIPSI  = "08";
  static const QByteArray RelatedToIGIC  = RelatedToIPSI;
}

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

static QDomElement generateEmitterXml(QDomDocument& document, const TbaiInvoiceInterface&)
{
  QDomElement root   = document.createElement("Emisor");
  QDomElement cifEl  = document.createElement("NIF");
  QDomElement nameEl = document.createElement("ApellidosNombreRazonSocial");

  cifEl.appendChild(document.createTextNode(CompanyData::self.cif));
  nameEl.appendChild(document.createTextNode(CompanyData::self.name));
  root.appendChild(cifEl);
  root.appendChild(nameEl);
  return root;
}

static QDomElement generateRecipientXml(QDomDocument& document, const CompanyData& recipient)
{
  const auto  cif         = recipient.cif;
  const auto  name        = recipient.name;
  QDomElement recipientEl = document.createElement("IDDestinatario");
  QDomElement cifEl       = document.createElement("NIF");
  QDomElement nameEl      = document.createElement("ApellidosNombreRazonSocial");
  QDomElement postalCodeEl= document.createElement("CodigoPostal");

  cifEl.appendChild(document.createTextNode(cif));
  nameEl.appendChild(document.createTextNode(name));
  recipientEl.appendChild(cifEl);
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
  codeEl.appendChild(document.createTextNode("S")); // S for Substitution, I for difference
  // Missing element(s): ImporteRectificacion and Sustitutiva, or ImporteRectificacionSustitutiva
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
    root.appendChild(generateCorrectionInvoiceHeader(document, invoice));
  return root;
}

static QDomElement generateInvoiceData(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root          = document.createElement("DatosFactura");
  QDomElement descriptionEl = document.createElement("DescripcionFactura");
  QDomElement amountEl      = document.createElement("ImporteTotalFactura");
  QDomElement keysEl        = document.createElement("Claves");
  QDomElement keyEl         = document.createElement("IDClave");
  QDomElement vatEl         = document.createElement("ClaveRegimenIvaOpTrascendencia");

  descriptionEl.appendChild(document.createTextNode(invoice.description().toUtf8()));
  amountEl.appendChild(document.createTextNode(invoice.formattedAmount()));
  keysEl.appendChild(keyEl);
    keyEl.appendChild(vatEl);
      vatEl.appendChild(document.createTextNode(VatPolicy::Default));
  root.appendChild(descriptionEl);
  root.appendChild(amountEl);
  root.appendChild(keysEl);
  return root;
}

static QDomElement generateVatInvoiceBreakdown(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root          = document.createElement("DetalleNoExenta");
  QDomElement typeEl        = document.createElement("TipoNoExenta");
  QDomElement vatEl         = document.createElement("DesgloseIVA");
  QDomElement vatDetailEl   = document.createElement("DetalleIVA");
  QDomElement taxableBaseEl = document.createElement("BaseImponible");

  // S1 -> Sin inversión del sujeto pasivo
  // S2 -> Con inversión del sujeto pasivo
  typeEl.appendChild(document.createTextNode("S1"));
  taxableBaseEl.appendChild(document.createTextNode(invoice.taxBaseAmount()));
  root.appendChild(typeEl);
  root.appendChild(vatEl);
  vatEl.appendChild(vatDetailEl);
  vatDetailEl.appendChild(taxableBaseEl);
  return root;
}

static QDomElement generateInvoiceBreakdown(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root = document.createElement("TipoDesglose");
  QDomElement wrapper = document.createElement("DesgloseFactura");
  QDomElement subjectEl;

  if (invoice.isSubjectToVat())
  {
    QDomElement exemptionEl;

    subjectEl = document.createElement("Sujeta");
    switch (invoice.vatExemption())
    {
    case TbaiInvoiceInterface::NoVatExemption:
      exemptionEl = document.createElement("NoExenta");
      exemptionEl.appendChild(generateVatInvoiceBreakdown(document, invoice));
      break ;
    default:
      exemptionEl = document.createElement("Exenta");
      throw std::runtime_error("Tax exemption not implemented in TbaiDocument");
    }
    subjectEl.appendChild(exemptionEl);
  }
  else
  {
    QDomElement notSubjectEl = document.createElement("DetalleNoSujeta");
    QDomElement causeEl      = document.createElement("Causa");
    QDomElement amountEl     = document.createElement("Importe");

    causeEl.appendChild(document.createTextNode(invoice.notSubjectToVatReason().toUtf8()));
    amountEl.appendChild(document.createTextNode(invoice.formattedAmount()));
    subjectEl = document.createElement("NoSujeta");
    subjectEl.appendChild(notSubjectEl);
    notSubjectEl.appendChild(causeEl);
    notSubjectEl.appendChild(amountEl);
  }
  wrapper.appendChild(subjectEl);
  root.appendChild(wrapper);
  return root;
}

static QDomElement generatePreviousInvoiceXml(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement root        = document.createElement("EncadenamientoFacturaAnterior");
  QDomElement serieEl     = document.createElement("SerieFacturaAnterior");
  QDomElement numberEl    = document.createElement("NumFacturaAnterior");
  QDomElement dateEl      = document.createElement("FechaExpedicionFacturaAnterior");
  QDomElement signatureEl = document.createElement("SignatureValueFirmaFacturaAnterior");

  if (invoice.series().length() > 0)
  {
    serieEl.appendChild(document.createTextNode(invoice.series()));
    root.appendChild(serieEl);
  }
  numberEl.appendChild(document.createTextNode(invoice.number()));
  dateEl.appendChild(document.createTextNode(invoice.date().toString("dd-MM-yyyy")));
  signatureEl.appendChild(document.createTextNode(invoice.signature()));
  root.appendChild(numberEl);
  root.appendChild(dateEl);
  root.appendChild(signatureEl);
  return root;
}

static QDomElement generateSoftwareXml(QDomDocument& document)
{
  QDomElement root        = document.createElement("Software");
  QDomElement licenseEl   = document.createElement("LicenciaTBAI");
  QDomElement developerEl = document.createElement("EntidadDesarrolladora");
  QDomElement cifEl       = document.createElement("NIF");
  QDomElement nameEl      = document.createElement("Nombre");
  QDomElement versionEl   = document.createElement("Version");
  QByteArray  license     = qgetenv("TBAI_LICENSE");
  QByteArray  name        = qgetenv("TBAI_SOFTWARE_NAME");
  QByteArray  cif         = qgetenv("TBAI_SOFTWARE_NIF");

  licenseEl.appendChild(document.createTextNode(license));
  cifEl.appendChild(document.createTextNode(cif));
  nameEl.appendChild(document.createTextNode(name));
  versionEl.appendChild(document.createTextNode(TBAI_SOFTWARE_VERSION));

  root.appendChild(licenseEl);
  root.appendChild(developerEl);
    developerEl.appendChild(cifEl);
  root.appendChild(nameEl);
  root.appendChild(versionEl);
  return root;
}

static QDomElement pluralSubjectHintXml(QDomDocument& document, const TbaiInvoiceInterface& invoice)
{
  QDomElement element = document.createElement("VariosDestinatarios");
  QString text = invoice.recipients().size() > 1 ? "Y" : "N";

  element.appendChild(document.createTextNode(text));
  return element;
}

static QByteArray getDeviceUid()
{
  return "AAAAAAA";
}

TbaiDocument& TbaiDocument::createFrom(const TbaiInvoiceInterface& invoice)
{
  clear();
  root = createElement("T:TicketBai");
  root.setAttribute("xmlns:T", "urn:ticketbai:emision");

  QDomElement headerEl        = createElement("Cabecera");
  QDomElement subjectsEl      = createElement("Sujetos");
  QDomElement invoiceEl       = createElement("Factura");
  QDomElement footPrintEl     = createElement("HuellaTBAI");
  QDomElement idVersionTbaiEl = createElement("IDVersionTBAI");
  QDomElement deviceIdEl      = createElement("NumSerieDispositivo");

  deviceIdEl.appendChild(createTextNode(getDeviceUid()));
  idVersionTbaiEl.appendChild(createTextNode(TBAI_VERSION));
  headerEl.appendChild(idVersionTbaiEl);
  root.appendChild(headerEl);

  subjectsEl.appendChild(generateEmitterXml(*this, invoice));
  if (invoice.recipients().size() > 0)
    subjectsEl.appendChild(generateRecipientXml(*this, invoice));
  if (invoice.recipients().size() > 1)
    subjectsEl.appendChild(pluralSubjectHintXml(*this, invoice));
  root.appendChild(subjectsEl);
  invoiceEl.appendChild(generateInvoiceHeader(*this, invoice));
  invoiceEl.appendChild(generateInvoiceData(*this, invoice));
  invoiceEl.appendChild(generateInvoiceBreakdown(*this, invoice));
  root.appendChild(invoiceEl);
  if (invoice.previousInvoice())
    root.appendChild(generatePreviousInvoiceXml(*this, *invoice.previousInvoice()));
  footPrintEl.appendChild(generateSoftwareXml(*this));
  footPrintEl.appendChild(deviceIdEl);
  root.appendChild(footPrintEl);
  appendChild(root);
  return *this;
}

bool TbaiDocument::loadFromFile(const QString& path)
{
  QFile file(path);

  if (file.open(QIODevice::ReadOnly) && loadFrom(file.readAll()))
    return true;
  qDebug() << "TbaiDocument::loadFromFile: could not load file" << path;
  return false;
}

bool TbaiDocument::loadFrom(const QByteArray& xml)
{
  QString errorMessage;
  int errorLine, errorColumn;

  clear();
  if (setContent(xml, false, &errorMessage, &errorLine, &errorColumn))
    root = elementsByTagName("T:TicketBai").item(0).toElement();
  else
    qDebug() << "TbaiDocument::loadFrom: xml parse error at" << errorLine << ':' << errorColumn << ':' << errorMessage;
  return !root.isNull();
}

void TbaiDocument::appendSignature(const QDomElement& signatureEl)
{
  root.appendChild(signatureEl);
}

bool TbaiDocument::isSigned() const
{
  return !root.elementsByTagName(signatureNamespace() + ":Signature").isEmpty();
}

QByteArray TbaiDocument::signature() const
{
  QDomNodeList matches = root.elementsByTagName(signatureNamespace() + ":SignatureValue");
  QDomNode signatureNode = matches.item(0);
  QString signature = signatureNode.lastChild().toText().data();

  if (signatureNode.isNull())
    throw std::runtime_error("TbaiDocument::getSignature: TicketBAI document is not signed.");
  if (matches.count() != 1)
    throw std::runtime_error("TbaiDocument::getSignature: TicketBAI document contains several signatures.");
  if (signature.length() == 0)
    throw std::runtime_error("TbaiDocument::getSignature: TicketBAI document contains a signature, but it is empty.");
  return signature.toUtf8();
}

static QString getDefaultFileNameFor(const TbaiInvoiceInterface& invoice)
{
  if (invoice.recipients().size() > 0)
  {
    const CompanyData& contact = invoice.recipients().first();

    return QUrl::toPercentEncoding(contact.cif + '-' + contact.name, " ");
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

QString TbaiDocument::signatureNamespace()
{
  return "ds";
}
