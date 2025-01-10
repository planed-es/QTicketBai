#ifndef  TBAIINVOICEINTERFACE_H
# define TBAIINVOICEINTERFACE_H

# include <QDateTime>
# include <QByteArray>
# include "tbaiinvoicedefines.h"
# include "companydata.h"

class TICKETBAIQT_EXPORT TbaiInvoiceInterface : public TbaiInvoiceDefines, public TbaiContactDefines
{
public:
  typedef CompanyData Recipient;
  typedef QList<Recipient> Recipients;
  typedef QList<TbaiInvoiceInterface*> TbaiInvoices;

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
  virtual CorrectionType        correctionType() const { return UndefinedCorrection; }
  virtual QList<VatRegime>      vatRegimes() const { return {DefaultRegime}; }
  virtual QList<VatBreakdown>   vatBreakdowns() const = 0;
  virtual const QByteArray&     signature() const = 0;
  virtual const QDateTime&      date() const = 0;
  virtual const QByteArray&     series() const = 0;
  virtual const QByteArray&     number() const = 0;
  virtual const QString&        name() const = 0;
  virtual const QString&        description() const = 0;
  virtual const Recipients&     recipients() const = 0;
};

#endif
