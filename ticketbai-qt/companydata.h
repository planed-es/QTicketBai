#ifndef  TBAIQT_COMPANYDATA_H
# define TBAIQT_COMPANYDATA_H

# include <QString>

enum TbaiIdentityType
{
  NifIvaId,
  PassportId,
  OficialIdentificationDocumentFromCountryOfResidenceId,
  ResidenceCertificateId,
  OtherSupportingDocumentId
};

struct CompanyData
{
  QString          name, address, city;
  TbaiIdentityType idType = NifIvaId;
  QString          id, phone, fax, postalCode, email;
  QByteArray       countryCode;

  bool operator==(const CompanyData& other) const { return idType == other.idType && id == other.id; }
};

#endif
