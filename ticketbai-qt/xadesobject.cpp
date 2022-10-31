#include "xadesobject.h"
#include "tbaicertificate.h"
#include <QDateTime>

TbaiXadesObject::TbaiXadesObject(QXmlSignContext context_) :
  signatureContext(context_),
  context(context_),
  signatureSpecsUrl("https://www.batuz.eus/fitxategiak/batuz/ticketbai/sinadura_elektronikoaren_zehaztapenak_especificaciones_de_la_firma_electronica_v1_0.pdf")
{
  signatureNamespace = context.nspace;
  context.nspace = "xades";
  qDebug() << "TbaiXadesObject" << signatureNamespace;
  m_qualifyingPropertiesId = context.tagId("QualifyingProperties");
  m_signedPropertiesId = context.tagId("SignedProperties");
}

TbaiXadesObject& TbaiXadesObject::addDataObjectFormat(const XadesObjectDataObjectFormat& val)
{
  dataObjectFormats.push_back(val);
  return *this;
}

QDomElement TbaiXadesObject::generate()
{
  QDomElement root = context.createElement("QualifyingProperties");
  QDomElement signedPropertiesEl = context.createElement("SignedProperties");

  signedPropertiesEl.appendChild(generateSignatureProperties());
  signedPropertiesEl.appendChild(generateDataObjectProperties());
  signedPropertiesEl.setAttribute("Id", signedPropertiesId());
  root.appendChild(signedPropertiesEl);
  root.setAttribute("Id", qualifyingPropertiesId());
  root.setAttribute("xmlns:" + context.nspace, "http://uri.etsi.org/01903/v1.3.2#");
  root.setAttribute("Target", '#' + context.signatureId);
  root.setAttribute("xmlns:" + signatureNamespace, "http://www.w3.org/2000/09/xmldsig");
  return root;
}

QDomElement TbaiXadesObject::generateSignatureProperties()
{
  QDomElement propertiesEl = context.createElement("SignedSignatureProperties");

  propertiesEl.appendChild(generateSigningTime());
  propertiesEl.appendChild(generateSigningCertificate());
  propertiesEl.appendChild(generatePolicyIdentifier());
  return propertiesEl;
}

QDomElement TbaiXadesObject::generateSigningTime()
{
  QDomElement signingTimeEl = context.createElement("SigningTime");
  QDateTime signingTime = QDateTime::currentDateTime();
  QString timezone =  "+0" + QString::number(signingTime.offsetFromUtc() / 60 / 60) + ":00";

  signingTimeEl.appendChild(context.document.createTextNode(QDateTime::currentDateTime().toString(Qt::ISODate) + timezone));
  return signingTimeEl;
}

QDomElement TbaiXadesObject::generateSigningCertificate()
{
  QDomElement root           = context.createElement("SigningCertificate");
  QDomElement certEl         = context.createElement("Cert");
  QDomElement certDigestEl   = context.createElement("CertDigest");
  QDomElement issuerSerialEl = context.createElement("IssuerSerial");
  QDomElement certDigestMethodEl = signatureContext.createElement("DigestMethod");
  QDomElement certDigestValueEl  = signatureContext.createElement("DigestValue");
  QDomElement issuerNameEl       = signatureContext.createElement("X509IssuerName");
  QDomElement serialNumberEl     = signatureContext.createElement("X509SerialNumber");

  certDigestMethodEl.setAttribute("Algorithm", "http://www.w3.org/2001/04/xmlenc#sha512");
  certDigestEl.appendChild(certDigestMethodEl);
  certDigestEl.appendChild(certDigestValueEl);
  issuerNameEl.appendChild(context.document.createTextNode(TbaiCertificate::issuerName()));
  serialNumberEl.appendChild(context.document.createTextNode(TbaiCertificate::serialNumber()));
  issuerSerialEl.appendChild(issuerNameEl);
  issuerSerialEl.appendChild(serialNumberEl);
  certEl.appendChild(certDigestEl);
  certEl.appendChild(issuerSerialEl);
  root.appendChild(certEl);
  return root;
}

QDomElement TbaiXadesObject::generatePolicyIdentifier()
{
  QDomElement root = context.createElement("SignaturePolicyIdentifier");
  QDomElement policyId = context.createElement("SignaturePolicyId");
  QDomElement sigPolicyId = context.createElement("SigPolicyId");
  QDomElement sigPolicyHash = context.createElement("SigPolicyHash");
  QDomElement sigPolicyQualifiers = context.createElement("SigPolicyQualifiers");

  {
    QDomElement identifier = context.createElement("Identifier");
    QDomElement description = context.createElement("Description");

    identifier.appendChild(context.document.createTextNode(signatureSpecsUrl.toString()));
    sigPolicyId.appendChild(identifier);
    sigPolicyId.appendChild(description);
  }

  {
    QDomElement digestMethod = signatureContext.createElement("DigestMethod");
    QDomElement digestValue  = signatureContext.createElement("DigestValue");

    digestMethod.setAttribute("Algorithm", "http://www.w3.org/2001/04/xmlenc#sha256");
    sigPolicyHash.appendChild(digestMethod);
    sigPolicyHash.appendChild(digestValue);
  }

  {
    QDomElement qualifier = context.createElement("SigPolicyQualifier");
    QDomElement spuri = context.createElement("SPURI");

    spuri.appendChild(context.document.createTextNode(signatureSpecsUrl.toString()));
    qualifier.appendChild(spuri);
    sigPolicyQualifiers.appendChild(qualifier);
  }

  policyId.appendChild(sigPolicyId);
  policyId.appendChild(sigPolicyHash);
  policyId.appendChild(sigPolicyQualifiers);
  root.appendChild(policyId);
  return root;
}

QDomElement TbaiXadesObject::generateDataObjectProperties()
{
  QDomElement objectPropertiesEl = context.createElement("SignedDataObjectProperties");

  for (const auto& dataObjectFormat : dataObjectFormats)
    objectPropertiesEl.appendChild(dataObjectFormat.generate(context));
  return objectPropertiesEl;
}
