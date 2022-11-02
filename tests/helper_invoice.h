#ifndef  HELPER_INVOICEH_H
# define HELPER_INVOICE_H

# include "ticketbai-qt/invoiceinterface.h"

class InvoiceTest : public TbaiInvoiceInterface
{
public:
  QDateTime m_date = QDateTime::currentDateTime();
  QByteArray m_signature, m_series, m_number;
  QString m_name;
  Recipients m_recipients = Recipients{
    CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", "A12345678", "0505050505", "0606060606", "12345", "e@ma.il"}
  };

  TbaiInvoiceInterface* previousInvoice() const override { return nullptr; }
  Type invoiceType() const override { return InvoiceType; }
  const QByteArray& signature() const override { return m_signature; }
  const QDateTime&  date() const override { return m_date; }
  const QByteArray& series() const override { return m_series; }
  const QByteArray& number() const override { return m_number; }
  const QString&    name() const override { return m_name; }
  const QString&    description() const override { return m_name; }
  double            amount() const override { return 42.42; }
  QString           notSubjectToVatReason() const override { return ""; }
  const Recipients& recipients() const override { return m_recipients; }
};

#endif
