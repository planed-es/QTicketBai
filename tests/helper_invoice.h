#ifndef  HELPER_INVOICEH_H
# define HELPER_INVOICE_H

# include "ticketbai-qt/invoiceinterface.h"

class InvoiceTest : public TbaiInvoiceInterface
{
public:
  TbaiInvoiceInterface* m_previousInvoice = nullptr;
  QDateTime m_date = QDateTime::currentDateTime();
  QByteArray m_signature, m_series, m_number;
  QString m_name;
  Recipients m_recipients = Recipients{
    CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", NifIvaId, "U39072079", "0505050505", "0606060606", "12345", "e@ma.il"}
  };
  QList<VatBreakdown> m_breakdowns = QList<VatBreakdown>{
    VatBreakdown{
      SubjectToVat,
      NoVatExemption,
      WithoutInversionOfPassiveSubject,
      42.42,
      0.21,
      0.014
    }
  };

  TbaiInvoiceInterface* previousInvoice() const override { return m_previousInvoice; }
  Type invoiceType() const override { return InvoiceType; }
  const QByteArray& signature() const override { return m_signature; }
  const QDateTime&  date() const override { return m_date; }
  const QByteArray& series() const override { return m_series; }
  const QByteArray& number() const override { return m_number; }
  const QString&    name() const override { return m_name; }
  const QString&    description() const override { return m_name; }
  double            amount() const override
  {
    double total = 0;
    for (const auto& i : m_breakdowns) total += i.total();
    return total;
  }
  const Recipients& recipients() const override { return m_recipients; }
  QList<VatBreakdown> vatBreakdowns() const override { return m_breakdowns; }
};

class InvoiceCorrectionTest : public TbaiInvoiceInterface
{
public:
  TbaiInvoiceInterface* m_previousInvoice = nullptr;
  QDateTime m_date = QDateTime::currentDateTime();
  QByteArray m_signature, m_series, m_number;
  QString m_name;
  Recipients m_recipients = Recipients{
    CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", NifIvaId, "U39072079", "0505050505", "0606060606", "12345", "e@ma.il"}
  };
  QList<VatBreakdown> m_breakdowns = QList<VatBreakdown>{
    VatBreakdown{
      SubjectToVat,
      NoVatExemption,
      WithoutInversionOfPassiveSubject,
      23.23,
      0.21
    }
  };
  QList<TbaiInvoiceInterface*> m_corrected;

  TbaiInvoiceInterface* previousInvoice() const override { return m_previousInvoice; }
  TbaiInvoices correctedInvoices() const override { return m_corrected; }
  Type invoiceType() const override { return CorrectionArt80_1_Type; }
  const QByteArray& signature() const override { return m_signature; }
  const QDateTime&  date() const override { return m_date; }
  const QByteArray& series() const override { return m_series; }
  const QByteArray& number() const override { return m_number; }
  const QString&    name() const override { return m_name; }
  const QString&    description() const override { return m_name; }
  double            amount() const override { return 23; }
  const Recipients& recipients() const override { return m_recipients; }
  QList<VatBreakdown> vatBreakdowns() const override { return m_breakdowns; }
};

#endif
