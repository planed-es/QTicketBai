#ifndef  TBAIINVOICEINTERFACE_H
# define TBAIINVOICEINTERFACE_H

# include <QDateTime>
# include <QByteArray>
# include "companydata.h"

class TbaiInvoiceInterface
{
public:
  typedef QList<CompanyData> Recipients;

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

  enum Type
  {
    InvoiceType = 0,
    CorrectionArt80_1_Type,
    CorrectionArt80_2_Type,
    CorrectionArt80_3_Type,
    CorrectionArt80_4_Type,
    CorrectionArt80_6_Type,
    CorrectionOthers_Type,
    SimplifiedCorrectionType
  };

  virtual TbaiInvoiceInterface* getPreviousInvoice() const = 0;
  virtual Type              getInvoiceType() const = 0;
  virtual const QByteArray& getSignature() const = 0;
  virtual const QDateTime&  getDate() const = 0;
  virtual QByteArray        getSeries() const { return QByteArray(); };
  virtual const QByteArray& getNumber() const = 0;
  virtual const QString&    getName() const = 0;
  virtual const QString&    getDescription() const = 0;
  virtual double            getAmount() const = 0;
  QByteArray                getFormattedAmount() const { return QByteArray::number(getAmount(), 'f', 2); }
  virtual QByteArray        getTaxBaseAmount() const { return getFormattedAmount(); }
  virtual Recipients        getRecipients() const = 0;
  virtual VatExemption      getVatExemption() const { return NoVatExemption; }
  virtual bool              isSubjectToVat() const { return getNotSubjectToVatReason().isEmpty(); }
  virtual QString           getNotSubjectToVatReason() const = 0;
};

#endif
