#ifndef  INVOICE_H
# define INVOICE_H

# include "invoiceline.h"
# include <QQmlListProperty>
# include <QUrl>

class TbaiContext;

class Invoice : public QObject, public TbaiInvoiceInterface
{
  Q_OBJECT

  Q_PROPERTY(QString    recipientId MEMBER _recipientId NOTIFY recipientChanged)
  Q_PROPERTY(QDateTime  date        MEMBER _date        NOTIFY changed)
  Q_PROPERTY(QByteArray series      MEMBER _series      NOTIFY changed)
  Q_PROPERTY(QByteArray number      MEMBER _number      NOTIFY changed)
  Q_PROPERTY(QString    name        MEMBER _name        NOTIFY changed)
  Q_PROPERTY(QString    description MEMBER _description NOTIFY changed)
  Q_PROPERTY(double     amount      READ amount         NOTIFY amountChanged)
  Q_PROPERTY(double     base        READ base           NOTIFY amountChanged)
  Q_PROPERTY(QByteArray signature   MEMBER _signature   NOTIFY signatureChanged)
  Q_PROPERTY(QString    xml         MEMBER _xml         NOTIFY signatureChanged)
  Q_PROPERTY(QUrl       qrcode      READ qrcode         NOTIFY signatureChanged)
  Q_PROPERTY(QUrl       tbaiUrl     READ tbaiUrl        NOTIFY signatureChanged)
  Q_PROPERTY(bool       isSigned    READ isSigned       NOTIFY signatureChanged)
  Q_PROPERTY(QQmlListProperty<InvoiceLine> lines READ qmlLines NOTIFY linesChanged)
public:
  explicit Invoice(const TbaiContext&, QObject* parent = nullptr);

  Q_INVOKABLE InvoiceLine* addLine();
  Q_INVOKABLE void removeLine(InvoiceLine*);

  TbaiInvoiceInterface* previousInvoice() const override;
  Type                  invoiceType() const override { return InvoiceType; }
  QList<VatBreakdown>   vatBreakdowns() const override;
  const QDateTime&      date() const override { return _date; }
  const QByteArray&     series() const override { return _series; }
  const QByteArray&     number() const override { return _number; }
  const QString&        name() const override { return _name; }
  const QString&        description() const override { return _description; }
  const QByteArray&     signature() const override { return _signature; }
  const Recipients&     recipients() const override { return _recipients; }
  Recipient&            recipient() { return _recipients.first(); }
  double                base() const;

  QUrl tbaiUrl() const;
  QUrl qrcode() const;
  bool isSigned() const { return _signature.length() > 0; }

  QQmlListProperty<InvoiceLine> qmlLines();

signals:
  void changed();
  void recipientChanged();
  void amountChanged();
  void signatureChanged();
  void linesChanged();

private slots:
  void onRecipientChanged();

private:
  const TbaiContext& _context;
  QList<InvoiceLine*> _lines;
  QDateTime _date;
  QByteArray _series, _number;
  QString _name, _description;
  QByteArray _signature;
  Recipients _recipients;
  QString _recipientId;
  QString _xml;
};

#endif
