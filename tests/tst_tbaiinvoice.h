#ifndef  TST_TBAIINVOICE_H
# define TST_TBAIINVOICE_H

# include <QObject>
# include <QTest>
# include "helper_tbai_test.h"

class TbaiInvoiceTest : public QTicketBaiTest
{
  Q_OBJECT
private slots:
  void generatesValidTbaiId();
  void generatesValidUrl();
};

#endif
