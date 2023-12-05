#include <QProcess>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QSslSocket>
#include "qticketbai.h"
#include "tbaisignprocess.h"
#include "tbaidocument.h"
#include "tbaicertificate.h"
#ifdef TICKETBAIQT_WITH_SIGNING
# include "xmlsec-qt/xmlverify.h"
# include "xmlsec-qt/xmlsign.h"
# include "xmlsec-qt/xades/object.h"
#endif
#include <iostream>
#include <QDebug>

bool TbaiSignProcess::checkSettings(const TbaiContext& context)
{
  const auto& certificate = context.constCertificate();
  const auto& software = context.constSoftware();

#define TBAI_FILE_CHECKER(path) \
  if (!path.isEmpty() && QFile::exists(path)) \
    std::cerr << "using " << path.toStdString() << std::endl; \
  else \
  { \
    std::cerr << "not found." << std::endl; \
    return false; \
  }

  std::cerr << "Checking configuration for QTicketBAI:" << std::endl;
  std::cerr << "- Using TicketBAI software CIF:\t\t" <<  software.cif().toStdString() << std::endl;
  std::cerr << "- Using TicketBAI software:\t\t" << software.name().toStdString() << std::endl;
  std::cerr << "- Using TicketBAI Tax Authority:\t" << context.taxAuthorityUrl().toString().toStdString() << std::endl;
  std::cerr << "- Qt supports SSL:\t" << (QSslSocket::supportsSsl() ? "Yes" : "No") << std::endl;
  std::cerr << "- Looking for certificate:\t\t";
  TBAI_FILE_CHECKER(certificate.path())
  std::cerr << "- Certificate password ?\t\t" << (certificate.password().isEmpty() ? "No" : "Yes") << std::endl;
  std::cerr << "- Certificate import:\t\t\t" << (certificate.isReady() ? "Success" : "Failure") << std::endl;
  return certificate.isReady();
}

TbaiSignProcess::ReturnValue TbaiSignProcess::sign(TbaiDocument& document)
{
  return sign(QTicketBai::context().constCertificate(), document);
}

TbaiSignProcess::ReturnValue TbaiSignProcess::sign(const TbaiCertificate& tbaiCertificate, TbaiDocument& document)
{
#ifdef TICKETBAIQT_WITH_SIGNING
  ReturnValue retval;
  QXmlSign signer;
  QXmlSecCertificate certificate;

  certificate.setFormat(QXmlSecCertificate::Pkcs12);
  certificate.setFilepath(tbaiCertificate.path());
  certificate.setPassword(tbaiCertificate.password());
  certificate.setName("QTicketBai/pkcs12");

  signer.withSignatureId("Signature")
        .useNamespace(TbaiDocument::signatureNamespace())
        .useDocument(document)
        .useSslKey(tbaiCertificate.sslKey(), tbaiCertificate.password().toUtf8())
        .useCertificate(certificate);

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
      .useV2()
      .useDigestAlgorithm(QCryptographicHash::Sha256)
      .useCertificate(tbaiCertificate.certificate());

  xadesObject.signedProperties().signatureProperties()
    .signaturePolicyIdentifier()
      .useIdentifier(specificationPdf)
      .useDigestAlgorithm(QCryptographicHash::Sha256)
      .useDigestValue("Quzn98x3PMbSHwbUzaj5f5KOpiH0u8bvmwbbbNkO9Es=")
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
  if (signer.sign())
  {
    document.loadFrom(signer.toString().toUtf8());
    retval.signature = document.signature();
    retval.xml = signer.toString().toUtf8();
  }
  else
    retval.error = "QXmlSign failed to sign the document";
  return retval;
#else
  return ReturnValue("QTicketBAI was built without document signing support");
#endif
}
