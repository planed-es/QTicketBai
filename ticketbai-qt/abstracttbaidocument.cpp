#include "abstracttbaidocument.h"
#include "tbaiinvoiceinterface.h"
#include "qticketbai.h"
#include <QFile>

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
typedef QDomDocument::ParseResult XmlParseResult;
#else
struct XmlParseResult
{
  int errorLine, errorColumn;
  QString errorMessage;
};
#endif

bool AbstractTbaiDocument::loadFromFile(const QString& path)
{
  QFile file(path);

  if (file.open(QIODevice::ReadOnly) && loadFrom(file.readAll()))
    return true;
  qDebug() << "AbstractTbaiDocument::loadFromFile: could not load file" << path;
  return false;
}

bool AbstractTbaiDocument::loadFrom(const QByteArray& xml)
{
  QString rootTag = "T:" + documentElementType();
  QString errorMessage;
  int errorLine, errorColumn;
  XmlParseResult result;

  clear();
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  if (result = setContent(xml))
#else
  if (setContent(xml, false, &result.errorMessage, &result.errorLine, &result.errorColumn))
#endif
    root = elementsByTagName(rootTag).item(0).toElement();
  else
    qDebug() << "AbstractTbaiDocument::loadFrom: xml parse error at" << result.errorLine << ':' << result.errorColumn << ':' << result.errorMessage;
  return !root.isNull();
}

void AbstractTbaiDocument::prepareDocument()
{
  QDomElement idVersionTbaiEl = createElement("IDVersionTBAI");

  headerEl = createElement("Cabecera");
  root = createElement("T:" + documentElementType());
  root.setAttribute("xmlns:T", QString(documentXmlns()));
  idVersionTbaiEl.appendChild(createTextNode(TBAI_VERSION));
  headerEl.appendChild(idVersionTbaiEl);
  root.appendChild(headerEl);
  appendChild(root);
}

const TbaiContext& AbstractTbaiDocument::context() const
{
  return !_context ? QTicketBai::context() : *_context;
}

QDomElement AbstractTbaiDocument::generateEmitter()
{
  QDomElement root   = createElement("Emisor");
  QDomElement cifEl  = createElement("NIF");
  QDomElement nameEl = createElement("ApellidosNombreRazonSocial");
  const auto& emitter = context().constEmitter();

  cifEl.appendChild(createTextNode(emitter.id()));
  nameEl.appendChild(createTextNode(emitter.name()));
  root.appendChild(cifEl);
  root.appendChild(nameEl);
  return root;
}

static const QMap<TbaiIdentityType, QByteArray> identityTypeCodes = {
  {TbaiContactDefines::NifIvaId,                                              "02"},
  {TbaiContactDefines::PassportId,                                            "03"},
  {TbaiContactDefines::OficialIdentificationDocumentFromCountryOfResidenceId, "04"},
  {TbaiContactDefines::ResidenceCertificateId,                                "05"},
  {TbaiContactDefines::OtherSupportingDocumentId,                             "06"}
};

QDomElement generateContactXml(QDomDocument& document, const CompanyData& contact)
{
  QDomElement idEl;

  if (contact.idType == TbaiContactDefines::NifIvaId)
  {
    idEl = document.createElement("NIF");
    idEl.appendChild(document.createTextNode(contact.id));
  }
  else
  {
    QDomElement idValueEl = document.createElement("ID");
    QDomElement idTypeEl  = document.createElement("IDType");
    QDomElement countryEl = document.createElement("CodigoPais");

    idEl = document.createElement("IDOtro");
    if (contact.countryCode.length() > 0)
    {
      countryEl.appendChild(document.createTextNode(contact.countryCode));
      idEl.appendChild(countryEl);
    }
    idTypeEl.appendChild(document.createTextNode(identityTypeCodes[contact.idType]));
    idValueEl.appendChild(document.createTextNode(contact.id));
    idEl.appendChild(idTypeEl);
    idEl.appendChild(idValueEl);
  }
  return idEl;
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
  signatureEl.appendChild(document.createTextNode(invoice.signature().left(100)));
  root.appendChild(numberEl);
  root.appendChild(dateEl);
  root.appendChild(signatureEl);
  return root;
}

static QDomElement generateSoftwareXml(QDomDocument& document, const TbaiContext& context)
{
  QDomElement root        = document.createElement("Software");
  QDomElement licenseEl   = document.createElement("LicenciaTBAI");
  QDomElement developerEl = document.createElement("EntidadDesarrolladora");
  QDomElement nameEl      = document.createElement("Nombre");
  QDomElement versionEl   = document.createElement("Version");

  licenseEl.appendChild(document.createTextNode(context.constSoftware().license()));
  nameEl.appendChild(document.createTextNode(context.constSoftware().name()));
  versionEl.appendChild(document.createTextNode(TBAI_SOFTWARE_VERSION));

  developerEl.appendChild(generateContactXml(document, context.constDeveloper().asCompanyData()));
  root.appendChild(licenseEl);
  root.appendChild(developerEl);
  root.appendChild(nameEl);
  root.appendChild(versionEl);
  return root;
}

QDomElement AbstractTbaiDocument::generateFingerprint(const TbaiInvoiceInterface& invoice)
{
  QDomElement footPrintEl = createElement("HuellaTBAI");
  QDomElement deviceIdEl  = createElement("NumSerieDispositivo");
  auto deviceUid = context().constSoftware().deviceUid();

  if (invoice.previousInvoice() && includePreviousInvoiceInFingerprint)
    footPrintEl.appendChild(generatePreviousInvoiceXml(*this, *invoice.previousInvoice()));
  footPrintEl.appendChild(generateSoftwareXml(*this, context()));
  if (deviceUid.length())
  {
    deviceIdEl.appendChild(createTextNode(deviceUid));
    footPrintEl.appendChild(deviceIdEl);
  }
  return footPrintEl;
}

QString AbstractTbaiDocument::signatureNamespace()
{
  return "ds";
}

void AbstractTbaiDocument::appendSignature(const QDomElement& signatureEl)
{
  root.appendChild(signatureEl);
}

bool AbstractTbaiDocument::isSigned() const
{
  return !root.elementsByTagName(signatureNamespace() + ":Signature").isEmpty();
}

QByteArray AbstractTbaiDocument::signature() const
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
