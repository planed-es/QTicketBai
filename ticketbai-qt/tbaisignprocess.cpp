#include <QProcess>
#include <QTemporaryFile>
#include <QStandardPaths>
#include "tbaisignprocess.h"
#include "tbaidocument.h"
#include "tbaicertificate.h"
#include "xmlsec-qt/xmlsign.h"
#include "xmlsec-qt/xades/object.h"
#include <iostream>
#include <QDebug>

TbaiSignProcess::TbaiSignProcess(QObject *parent) : QObject(parent)
{
  connect(this, &TbaiSignProcess::failed,       this, &TbaiSignProcess::finished, Qt::QueuedConnection);
  connect(this, &TbaiSignProcess::generatedXml, this, &TbaiSignProcess::finished, Qt::QueuedConnection);
}

bool TbaiSignProcess::checkSettings()
{
  QByteArray certificate = qgetenv("TBAI_CERTIFICATE_PATH");

#define TBAI_FILE_CHECKER(path) \
  if (!path.isEmpty() && QFile::exists(path)) \
    std::cerr << "using " << path.toStdString() << std::endl; \
  else \
  { \
    std::cerr << "not found." << std::endl; \
    return false; \
  }

  std::cerr << "Checking configuration for QTicketBAI:" << std::endl;
  std::cerr << "- Looking for certificate:\t";
  TBAI_FILE_CHECKER(TbaiCertificate::path())
  std::cerr << "- Certificate password ?\t" << (TbaiCertificate::password().isEmpty() ? "No" : "Yes") << std::endl;
  std::cerr << "- Using TicketBAI software CIF:\t" << qgetenv("TBAI_SOFTWARE_CIF").toStdString() << std::endl;
  std::cerr << "- Using TicketBAI software:\t" << qgetenv("TBAI_SOFTWARE_NAME").toStdString() << std::endl;
  std::cerr << "- Using TicketBAI Tax Authority:\t" << qgetenv("TBAI_TAX_AUTHORITY_URL").toStdString() << std::endl;
  return true;
}

void TbaiSignProcess::sign(const TbaiInvoiceInterface& invoice)
{
  bool success;
  QXmlSign signer;
  QXmlSecCertificate certificate;

  document.createFrom(invoice);
  signer.withSignatureId("Signature")
        .useNamespace(TbaiDocument::signatureNamespace())
        .useDocument(document);
  certificate.setFormat(QXmlSecCertificate::Pkcs12);
  certificate.setFilepath(TbaiCertificate::path());
  certificate.setPassword(TbaiCertificate::password());
  certificate.setName("QTicketBai/pkcs12");

  QString keyInfoId              = signer.signatureContext().tagId("KeyInfo");
  QString signatureSignInfoRefId = signer.signatureContext().tagId("Reference");

  /*
   * <Object/>
   */
  QXadesObject xadesObject(signer.signatureContext());
  QUrl specificationPdf("https://www.batuz.eus/fitxategiak/batuz/ticketbai/sinadura_elektronikoaren_zehaztapenak_especificaciones_de_la_firma_electronica_v1_0.pdf");

  xadesObject.useNamespace("xades");
  xadesObject.signedProperties().signatureProperties()
    .useSigningTime()
    .signingCertificate()
      .useDigestAlgorithm(QCryptographicHash::Sha512)
      .useCertificate(TbaiCertificate::certificate);

  xadesObject.signedProperties().signatureProperties()
    .signaturePolicyIdentifier()
      .useIdentifier(specificationPdf)
      .useDigestAlgorithm(QCryptographicHash::Sha256)
      .useDigestValue("Quzn98x3PMbSHwbUzaj5f5KOpiH0u8bvmwbbbNkO9Es")
      .addQualifier(
        QXadesSignaturePolicyQualifier().useUrl(specificationPdf)
      );

  xadesObject.signedProperties().addDataObjectFormat(
    QXadesDataObjectFormat(signatureSignInfoRefId)
      .withIdentifierQualifier("OIDAsURN")
      .withIdentifier("urn:oid:1.2.840.10003.5.109.10")
      .withMimetype("text/xml")
  );

  signer.addObject(xadesObject.generate());

  /*
   * <SignInfo/>
   */
  signer.useSignInfo(QXmlSignInfo()
    .addReference(QXmlSignReference()
      .withId(signatureSignInfoRefId)
      .withType(QUrl("http://www.w3.org/2000/09/xmldsig#Object"))
      .withUri("")
      .useAlgorithm(QUrl("http://www.w3.org/2001/04/xmlenc#sha512"))
      //.addTransform(QUrl("http://www.w3.org/TR/2001/REC-xml-c14n-20010315"))
      .addTransform(QUrl("http://www.w3.org/2000/09/xmldsig#enveloped-signature"))
      .addTransform(QUrl("http://www.w3.org/TR/1999/REC-xpath-19991116"))
    )
    .addReference(QXmlSignReference()
      .withUri('#' + xadesObject.signedPropertiesId())
      .withType(QUrl("http://uri.etsi.org/01903#SignedProperties"))
      .useAlgorithm(QUrl("http://www.w3.org/2001/04/xmlenc#sha512"))
    )
    .addReference(QXmlSignReference()
      .withUri('#' + keyInfoId)
      .useAlgorithm(QUrl("http://www.w3.org/2001/04/xmlenc#sha512"))
    )
  );

  /*
   * <KeyInfo/>
   */
  signer.useKeyInfo(QXmlSignKeyInfo()
    .withId(keyInfoId)
    .withKeyValue()
    .withX509Data()
  );

  /*
   * Signing
   */
  if (signer.sign(certificate))
  {
    document.loadFrom(signer.toString().toUtf8());
    emit generatedSignature(document.getSignature());
    emit generatedXml(signer.toString().toUtf8());
  }
  else
    emit failed("QXmlSign failed to sign the document");
}
