#ifndef  TBAIINVOICECOMPONENT_H
# define TBAIINVOICECOMPONENT_H

# include <QObject>
# include "ticketbai-qt_global.h"
# include "invoiceinterface.h"

class TICKETBAIQT_EXPORT TbaiInvoiceComponent : public QObject, public TbaiInvoiceInterface
{
  Q_OBJECT
  Q_ENUM(VatExemption)
  Q_ENUM(Type)

  Q_PROPERTY(Type         invoiceType READ invoiceType WRITE setInvoiceType NOTIFY invoiceTypeChanged)
  Q_PROPERTY(QByteArray   signature READ signature NOTIFY signatureChanged)
  Q_PROPERTY(QDateTime    date READ date WRITE setDate NOTIFY dateChanged)
  Q_PROPERTY(QByteArray   series READ series WRITE setSeries NOTIFY seriesChanged)
  Q_PROPERTY(QByteArray   number READ number WRITE setNumber NOTIFY numberChanged)
  Q_PROPERTY(QString      name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString      description READ description WRITE setDescription NOTIFY descriptionChanged)
  Q_PROPERTY(double       amount READ amount WRITE setAmount NOTIFY amountChanged)
  Q_PROPERTY(QByteArray   formattedAmount READ formattedAmount NOTIFY amountChanged)
  Q_PROPERTY(VatExemption vatExemption READ vatExemption WRITE setVatExemption NOTIFY vatExemptionChanged)
public:
  explicit TbaiInvoiceComponent(QObject* parent = nullptr) : QObject(parent) {}

  virtual TbaiInvoiceInterface* previousInvoice() const override { return m_previousInvoice; }

  virtual Type invoiceType() const override { return m_invoiceType; }
  virtual void setInvoiceType(Type value) { m_invoiceType = value; emit invoiceTypeChanged(); }

  const QByteArray& signature() const override { return m_signature; }
  void setSignature(const QByteArray& val) { m_signature = val; emit signatureChanged(); }

  const QDateTime& date() const override { return m_date; }
  void setDate(const QDateTime& val) { m_date = val; emit dateChanged(); }

  const QByteArray& series() const override { return m_series; }
  void setSeries(const QByteArray& val) { m_series = val; emit seriesChanged(); }

  const QByteArray& number() const override { return m_number; }
  void setNumber(const QByteArray& val) { m_number = val; emit numberChanged(); }

  const QString& name() const override { return m_name; }
  void setName(const QString& val) { m_name = val; emit nameChanged(); }

  const QString& description() const override { return m_description; }
  void setDescription(const QString& val) { m_description = val; emit descriptionChanged(); }

  double amount() const override { return m_amount; }
  void setAmount(double val) { m_amount = val; emit amountChanged(); }

  const Recipients& recipients() const { return m_recipients; }
  void addRecipient(const Recipient& val) { m_recipients << val; emit recipientsChanged(); }
  void removeRecipient(const Recipient& val) { m_recipients.removeAll(val); emit recipientsChanged(); }
  void setRecipients(const Recipients& val) { m_recipients = val; emit recipientsChanged(); }

  VatExemption vatExemption() const { return m_vatExemption; }
  void setVatExemption(VatExemption val) { m_vatExemption = val; emit vatExemptionChanged(); }

signals:
  void invoiceTypeChanged();
  void signatureChanged();
  void dateChanged();
  void seriesChanged();
  void numberChanged();
  void nameChanged();
  void descriptionChanged();
  void amountChanged();
  void recipientsChanged();
  void vatExemptionChanged();

private:
  Type         m_invoiceType;
  QByteArray   m_signature, m_number, m_series;
  QString      m_name, m_description;
  QDateTime    m_date;
  double       m_amount = 0;
  Recipients   m_recipients;
  VatExemption m_vatExemption = NoVatExemption;
  TbaiInvoiceInterface* m_previousInvoice = nullptr;
};

#endif
