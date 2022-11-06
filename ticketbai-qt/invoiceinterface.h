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
  typedef QList<TbaiInvoiceInterface*> TbaiInvoices;

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

  // L11
  enum NonExemptedVatType
  {
    WithInversionOfPassiveSubject,
    WithoutInversionOfPassiveSubject
  };

  // L13
  enum NotSubjectToVatReason
  {
    SubjectToVat,
    VatExemptedByNormaForalArticle7,
    VatExemptedDueToLocalization
  };

  struct VatBreakdown
  {
    NotSubjectToVatReason vatState        = SubjectToVat;
    VatExemption          exemptionType   = NoVatExemption;
    NonExemptedVatType    nonExemptedType = WithoutInversionOfPassiveSubject;
    double                base            = 0;
    double                taxRate         = 0;
    double                recargoRate     = 0;
    bool                  recargoSimplifiedRegime = false;
    double                taxFee()     const { return base * taxRate; }
    double                recargoFee() const { return base * recargoRate; }
    double                total()      const { return base + taxFee() + recargoFee(); }
  };

  virtual TbaiInvoiceInterface* previousInvoice() const = 0;
  virtual TbaiInvoices          correctedInvoices() const { return {}; }
  virtual Type                  invoiceType() const = 0;
  virtual QList<VatRegime>      vatRegimes() const { return {DefaultRegime}; }
  virtual QList<VatBreakdown>   vatBreakdowns() const = 0;
  virtual const QByteArray&     signature() const = 0;
  virtual const QDateTime&      date() const = 0;
  virtual const QByteArray&     series() const = 0;
  virtual const QByteArray&     number() const = 0;
  virtual const QString&        name() const = 0;
  virtual const QString&        description() const = 0;
  virtual const Recipients&     recipients() const = 0;
  virtual QString               formattedAmount() const { return QString::number(amount(), 'f', 2); }
  virtual double                amount() const
  {
    double total = 0;
    for (const VatBreakdown& breakdown : vatBreakdowns())
      total += breakdown.total();
    return total;
  }
};

#endif
