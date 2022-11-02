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
    SimplifiedCorrectionType,
    CancelType
  };

  virtual TbaiInvoiceInterface* previousInvoice() const = 0;
  virtual Type                  invoiceType() const = 0;
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
