#ifndef  TST_TBAIINVOICE_H
# define TST_TBAIINVOICE_H

# include <QObject>
# include <QTest>

class TbaiInvoiceTest : public QObject
{
  Q_OBJECT
private slots:
  void generatesValidTbaiId();
  void generatesValidUrl();
};

#endif
