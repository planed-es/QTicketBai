#ifndef  TBAIINVOICEINTERFACE_H
# define TBAIINVOICEINTERFACE_H

# include <QDateTime>
# include <QByteArray>
# include "companydata.h"

class TbaiInvoiceInterface
{
public:
  typedef CompanyData Recipient;
  typedef QList<Recipient> Recipients;

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

  virtual TbaiInvoiceInterface* previousInvoice() const = 0;
  virtual Type                  invoiceType() const = 0;
  virtual QList<VatRegime>      vatRegimes() const { return {DefaultRegime}; }
  virtual const QByteArray&     signature() const = 0;
  virtual const QDateTime&      date() const = 0;
  virtual const QByteArray&     series() const = 0;
  virtual const QByteArray&     number() const = 0;
  virtual const QString&        name() const = 0;
  virtual const QString&        description() const = 0;
  virtual double                amount() const = 0;
  QByteArray                    formattedAmount() const { return QByteArray::number(amount(), 'f', 2); }
  virtual QByteArray            taxBaseAmount() const { return formattedAmount(); }
  virtual const Recipients&     recipients() const = 0;
  virtual VatExemption          vatExemption() const { return NoVatExemption; }
  virtual bool                  isSubjectToVat() const { return notSubjectToVatReason().isEmpty(); }
  virtual QString               notSubjectToVatReason() const { return QString(); }
};

#endif
