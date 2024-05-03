#include "invoicecontroller.h"
#include <ticketbai-qt/context.h>
#include <ticketbai-qt/tbaisignprocess.h>
#include <ticketbai-qt/tbaidocument.h>

InvoiceController* InvoiceController::self = nullptr;

InvoiceController::InvoiceController(const TbaiContext& context, QObject* parent) : QObject(parent), _context(context)
{
  if (self != nullptr)
    throw std::logic_error("creating InvoiceController singleton twice");
  self = this;
}

InvoiceController::~InvoiceController()
{
  self = nullptr;
}

Invoice* InvoiceController::createInvoice()
{
  Invoice* invoice = new Invoice(_context, this);

  _invoices << invoice;
  emit listChanged();
  return invoice;
}

void InvoiceController::remove(Invoice* invoice)
{
  if (invoice)
  {
    if (invoice->signature() == QByteArray())
    {
      _invoices.removeAll(invoice);
      invoice->deleteLater();
      emit listChanged();
    }
    else
      emit cantRemoveSignedInvoice();
  }
}

bool InvoiceController::sign(Invoice* invoice)
{
  if (invoice)
  {
    auto* previous = previousInvoice(invoice);

    qDebug() << "Looking for previous invoice?" << invoice;
    if (previous && !previous->isSigned())
      sign(previous);
    if (TbaiSignProcess::checkSettings(_context))
    {
      TbaiDocument document;
      TbaiSignProcess::ReturnValue result;

      document.useContext(_context);
      document.createFrom(*invoice);

      result = TbaiSignProcess::sign(_context.constCertificate(), document);
      if (result)
      {
        invoice->setProperty("signature", result.signature);
        invoice->setProperty("xml", result.xml);
        return true;
      }
      else
        qDebug() << "Could not sign invoice";
    }
    else
      qDebug() << "Could not validate TicketBAI settings";
  }
  return false;
}

Invoice* InvoiceController::previousInvoice(const Invoice* invoice) const
{
  QList<Invoice*>::const_iterator result = _invoices.end();

  for (auto it = _invoices.begin() ; it != _invoices.end() ; ++it)
  {
    if (*it == invoice)
      return result != _invoices.end() ? *result : nullptr;
    result = it;
  }
  return nullptr;
}
