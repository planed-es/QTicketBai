#include "abstracttbaidocument.h"
#include "tbaiinvoiceinterface.h"
#include "qticketbai.h"

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
  QDomElement deviceIdEl      = createElement("NumSerieDispositivo");

  deviceIdEl.appendChild(createTextNode(context().constSoftware().deviceUid()));
  if (invoice.previousInvoice())
    footPrintEl.appendChild(generatePreviousInvoiceXml(*this, *invoice.previousInvoice()));
  footPrintEl.appendChild(generateSoftwareXml(*this, context()));
  footPrintEl.appendChild(deviceIdEl);
  return footPrintEl;
}
