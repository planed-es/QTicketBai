#include "invoice.h"
#include "invoicecontroller.h"
#include "contactcontroller.h"
#include <ticketbai-qt/tbaiinvoice.h>
#include <QBuffer>

Invoice::Invoice(const TbaiContext& context, QObject* parent) : QObject(parent), _context(context)
{
  _recipients << Recipient();
  _date = QDateTime::currentDateTime();
  connect(this, &Invoice::recipientChanged, this, &Invoice::onRecipientChanged);
}

double Invoice::base() const
{
  double total = 0;

  for (const auto* line : _lines)
    total += line->total();
  return total;
}

QUrl Invoice::tbaiUrl() const
{
  if (isSigned())
  {
    TbaiInvoice wrapper(_context, this);

    return wrapper.getUrl();
  }
  return QUrl();
}

QUrl Invoice::qrcode() const
{
  TbaiInvoice wrapper(_context, this);
  QImage      image = wrapper.getQRCode();
  QByteArray  output;
  QBuffer     buffer(&output);

  buffer.open(QIODevice::WriteOnly);
  image.save(&buffer, "png");
  qDebug() << "Generating qrcode image";
  image.save("/tmp/qrcode.png", "png");
  return QString("data:image/png;base64,") + QString::fromUtf8(output.toBase64());
}

TbaiInvoiceInterface* Invoice::previousInvoice() const
{
  return InvoiceController::self->previousInvoice(this);
}

InvoiceLine* Invoice::addLine()
{
  InvoiceLine* line = new InvoiceLine(this);

  connect(line, &InvoiceLine::amountChanged, this, &Invoice::amountChanged);
  _lines << line;
  emit linesChanged();
  return line;
}

void Invoice::removeLine(InvoiceLine* line)
{
  if (line)
  {
    _lines.removeAll(line);
    line->deleteLater();
    emit linesChanged();
  }
}

QQmlListProperty<InvoiceLine> Invoice::qmlLines()
{
  return QQmlListProperty<InvoiceLine>(this, &_lines);
}

static QList<TbaiInvoiceInterface::VatBreakdown>::iterator findMatchingBreakdown(QList<TbaiInvoiceInterface::VatBreakdown>& list, const InvoiceLine& line)
{
  const Product* product = line.product();

  if (product)
  {
    for (auto it = list.begin() ; it != list.end() ; ++it)
    {
      auto& breakdown = *it;

      if (breakdown.vatState == product->vatState() &&
          breakdown.exemptionType == product->vatExemption() &&
          breakdown.taxRate == line.taxRate())
        return it;
    }
  }
  return list.end();
}

static QList<TbaiInvoiceInterface::VatBreakdown>::iterator createBreakdown(QList<TbaiInvoiceInterface::VatBreakdown>& list, const InvoiceLine& line)
{
  const Product* product = line.product();

  if (product)
  {
    TbaiInvoiceInterface::VatBreakdown breakdown;

    breakdown.vatState      = product->vatState();
    breakdown.exemptionType = product->vatExemption();
    breakdown.taxRate       = line.taxRate() / 100;
    return list.insert(list.end(), breakdown);
  }
  return list.end();
}

static void addToBreakdown(TbaiInvoiceInterface::VatBreakdown& breakdown, const InvoiceLine& line)
{
  breakdown.base += line.total();
}

QList<TbaiInvoiceInterface::VatBreakdown> Invoice::vatBreakdowns() const
{
  QList<VatBreakdown> breakdowns;

  for (InvoiceLine* line : _lines)
  {
    auto it = findMatchingBreakdown(breakdowns, *line);

    if (it == breakdowns.end())
      it = createBreakdown(breakdowns, *line);
    if (it != breakdowns.end())
      addToBreakdown(*it, *line);
  }
  return breakdowns;
}

void Invoice::onRecipientChanged()
{
  const Contact* contact = ContactController::self->find(_recipientId);

  if (contact)
  {
    recipient() = contact->asCompanyData();
    emit changed();
  }
}
