#ifndef  TBAIINVOICE_H
# define TBAIINVOICE_H

# include "ticketbai-qt_global.h"
# include "tbaiinvoiceinterface.h"
# include <QtXml>
# ifdef QT_GUI_LIB
#  include <QImage>
# endif

class TbaiContext;

class TICKETBAIQT_EXPORT TbaiInvoice
{
public:
  TbaiInvoice(const TbaiContext& context, const TbaiInvoiceInterface* ptr);
  TbaiInvoice(const TbaiContext& context, const TbaiInvoiceInterface& ptr);
  TbaiInvoice(const TbaiInvoiceInterface* ptr);
  TbaiInvoice(const TbaiInvoiceInterface& ref);

  QByteArray getId() const;
  QByteArray getIdWithCRC() const;
  QUrl       getUrl() const;
# ifdef QT_GUI_LIB
  QImage     getQRCode() const;
#endif

private:
  QByteArray   generateCRC(const QByteArray& input) const;

  const TbaiContext&          context;
  const TbaiInvoiceInterface& invoice;
};

#endif // TBAIINVOICE_H
