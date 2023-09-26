#ifndef  TBAIINVOICE_H
# define TBAIINVOICE_H

# include "ticketbai-qt_global.h"
# include "invoiceinterface.h"
# include <QtXml>
# ifdef QT_GUI_LIB
#  include <QImage>
# endif

class TICKETBAIQT_EXPORT TbaiInvoice
{
public:
  TbaiInvoice(const TbaiInvoiceInterface* ptr) : invoice(*ptr) {}
  TbaiInvoice(const TbaiInvoiceInterface& ref) : invoice(ref) {}

  QByteArray getId() const;
  QByteArray getIdWithCRC() const;
  QUrl       getUrl() const;
# ifdef QT_GUI_LIB
  QImage     getQRCode() const;
#endif

private:
  QByteArray   generateCRC(const QByteArray& input) const;

  const TbaiInvoiceInterface& invoice;
};

#endif // TBAIINVOICE_H
