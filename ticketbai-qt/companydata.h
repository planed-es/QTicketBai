#ifndef  TBAIQT_COMPANYDATA_H
# define TBAIQT_COMPANYDATA_H

# include <QString>

struct CompanyData
{
  static const CompanyData self;
  QString name, address, city, cif, phone, fax, postalCode, email;
};

#endif
