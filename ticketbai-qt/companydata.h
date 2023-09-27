#ifndef  TBAIQT_COMPANYDATA_H
# define TBAIQT_COMPANYDATA_H

# include "tbaicontactdefines.h"
# include <QString>

typedef TbaiContactDefines::TbaiIdentityType TbaiIdentityType;

struct CompanyData
{
  QString          name, address, city, postalCode;
  TbaiIdentityType idType = TbaiContactDefines::NifIvaId;
  QString          id, phone, fax, email;
  QByteArray       countryCode;

  bool operator==(const CompanyData& other) const { return idType == other.idType && id == other.id; }
};

#endif
