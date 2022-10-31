#ifndef  TBAIXADESOBJECT_H
# define TBAIXADESOBJECT_H

# include "xadesobjectdataobjectformat.h"

class TbaiXadesObject
{
public:
  explicit TbaiXadesObject(QXmlSignContext);

  QString qualifyingPropertiesId() const { return m_qualifyingPropertiesId; }
  QString signedPropertiesId() const { return m_signedPropertiesId; }

  TbaiXadesObject& addDataObjectFormat(const XadesObjectDataObjectFormat& val);

  QDomElement generate();

private:
  QDomElement generateSignatureProperties();
  QDomElement generateSigningTime();
  QDomElement generateSigningCertificate();
  QDomElement generatePolicyIdentifier();
  QDomElement generateDataObjectProperties();

  QXmlSignContext signatureContext;
  QXmlSignContext context;
  QString signatureNamespace;
  QString m_qualifyingPropertiesId;
  QString m_signedPropertiesId;
  QUrl signatureSpecsUrl;

  QList<XadesObjectDataObjectFormat> dataObjectFormats;
};

#endif
