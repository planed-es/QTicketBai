#ifndef  TBAIINVOICEDEFINES_H
# define TBAIINVOICEDEFINES_H

# include <QObject>
# include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT TbaiInvoiceDefines
{
  Q_GADGET
public:
  TbaiInvoiceDefines();

  /*
  * The following enums are transcriptions of the types described in the
  * document "Especificaciones funcionales y técnicas del sistema TicketBAI"
  * Annexo I, part II (page 15).
  * https://www.batuz.eus/fitxategiak/batuz/normativa/2020%2009%2011%20ORDEN%20FORAL%201482-2020,%20de%209%20de%20septiembre.pdf
  */

  // L7
  enum Type
  {
    InvoiceType = 0,
    CorrectionArt80_1_Type,
    CorrectionArt80_2_Type,
    CorrectionArt80_3_Type,
    CorrectionArt80_4_Type,
    CorrectionArt80_6_Type,
    CorrectionOthers_Type,
    SimplifiedCorrectionType,
    CancelType
  };
  Q_ENUM(Type)

  // L9
  enum VatRegime
  {
    DefaultRegime,
    ExportationRegime,
    UsedGoodsRegime,
    GoldInvestmentRegime,
    TravelAgencyRegime,
    EntityGroupVatRegime,
    FundCriteraRegime,
    RelatedToIPSIOrIGICRegime,
    ProxyTravelAgencyRegime,
    CollectionsOnBehalfOfThirdPartiesOfProfessionalFeesOrRights,
    LeasingOperationsOfBusinessPremisesSubjectToWithholding,
    LeasingOperationsOfBusinessPremisesNotSubjectToWithholding,
    LeasingOperationsOfBusinessPremises,
    InvoiceWithVatPendingAccrualInWorkCertificationsWhoseRecipientIsAPublicAdministration,
    InvoiceWithVatPendingAccrualInSuccessiveTractOperations,
    OperationsInEquivalenceSurcharge,
    OperationsInSimplifiedRegime,
    OperationsCarriedOutByEntitiesNotConsideredBusinesspersonOrProfessionalsForVatPurposes
  };
  Q_ENUM(VatRegime)

  // L10
  enum VatExemption
  {
    NoVatExemption,
    VatExemptedByNormaForalArticle20,
    VatExemptedByNormaForalArticle21,
    VatExemptedByNormaForalArticle22,
    VatExemptedByNormaForalArticle23and24,
    VatExemptedByNormaForalArticle25,
    VatExempted
  };
  Q_ENUM(VatExemption)

  // L11
  enum NonExemptedVatType
  {
    WithInversionOfPassiveSubject,
    WithoutInversionOfPassiveSubject
  };
  Q_ENUM(NonExemptedVatType)

  // L13
  enum NotSubjectToVatReason
  {
    SubjectToVat,
    VatExemptedByNormaForalArticle7,
    VatExemptedDueToLocalization
  };
  Q_ENUM(NotSubjectToVatReason)
};

#endif
