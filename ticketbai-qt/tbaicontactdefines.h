#ifndef  TBAIQT_CONTACTDEFINES_H
# define TBAIQT_CONTACTDEFINES_H

# include <QObject>
# include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT TbaiContactDefines
{
  Q_GADGET
public:
  enum TbaiIdentityType
  {
    NifIvaId,
    PassportId,
    OficialIdentificationDocumentFromCountryOfResidenceId,
    ResidenceCertificateId,
    OtherSupportingDocumentId
  };
  Q_ENUM(TbaiIdentityType)
};

#endif
