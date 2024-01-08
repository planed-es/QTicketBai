#ifndef  HELPER_INVOICEH_H
# define HELPER_INVOICE_H

# include "ticketbai-qt/tbaiinvoiceinterface.h"
# include <random>

class InvoiceTest : public TbaiInvoiceInterface
{
public:
  TbaiInvoiceInterface* m_previousInvoice = nullptr;
  QDateTime m_date = QDateTime::currentDateTime();
  QByteArray m_signature, m_series, m_number;
  QString m_name;
  Recipients m_recipients = Recipients{
    CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", "12345", NifIvaId, "U39072079", "0505050505", "0606060606", "e@ma.il"}
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

  static QByteArray getRandomString()
  {
    const QByteArray possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12;
    QByteArray randomString;

    for(int i = 0 ; i < randomStringLength ; ++i)
    {
      int index = rand() % possibleCharacters.length();
      auto nextChar = possibleCharacters.at(index);
      randomString.push_back(nextChar);
    }
    return randomString;
  }

  template<typename INVOICE_TYPE = InvoiceTest>
  static INVOICE_TYPE factory(const QString& name = "Fish sale")
  {
    INVOICE_TYPE model;

    model.m_number = getRandomString();
    model.m_name = name;
    return model;
  }

  template<typename INVOICE_TYPE = InvoiceTest>
  static INVOICE_TYPE factory(const QString& name, TbaiInvoiceInterface& previous)
  {
    INVOICE_TYPE model = factory<INVOICE_TYPE>(name);

    model.m_previousInvoice = &previous;
    return model;
  }

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
    CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", "12345", NifIvaId, "U39072079", "0505050505", "0606060606", "e@ma.il"}
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

  static InvoiceCorrectionTest factory(const QString& name, TbaiInvoiceInterface& previous)
  {
    return InvoiceTest::factory<InvoiceCorrectionTest>(name, previous);
  }

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
