#include "tst_tbaiinvoice.h"
#include "ticketbai-qt/tbaiinvoice.h"
#include "ticketbai-qt/invoicecomponent.h"

QTEST_MAIN(TbaiInvoiceTest)

static QSharedPointer<TbaiInvoiceComponent> testInvoice()
{
  auto invoice = QSharedPointer<TbaiInvoiceComponent>(new TbaiInvoiceComponent);

  invoice->setDate(QDateTime::fromString("2022-12-25T04:10:24", Qt::ISODate));
  invoice->setSignature("DSI1XA1ErCoqaEln++216eTQpN0qF7HQb/I3QhF54V11LEGY3ucMVH33GceRE9LtYJI7AoI0YS3q0qPOn+eTlH6ZHVXXkcVZyi2gBZcuCKqRiRzSiuS0MazDgYqBdrDWhAIByP6AJ2vRhxhZceACeHFNzfmjeDpjX01cUox/RFT//C4cUOuTWADlkq/PqEN0dK/UQbZEOssk4diIlCeYU/exVYoRIpkE9OcFrlwOJv/UJ3ezBlL9pd/6G5rhNMtwqnPXLkbBq993uuVCU2SeKi79HuAa/gCvHlFwrjhwJV6VgcGhPylJ67xJk6dq+ukSW2s6kI4OgauH4SzYUaICAQ==");
  invoice->setNumber("FAC23");
  invoice->setAmount(7561);
  return invoice;
}

void TbaiInvoiceTest::generatesValidTbaiId()
{
  auto invoice = testInvoice();
  TbaiInvoice tbaiInvoice(*invoice);

  QCOMPARE(tbaiInvoice.getId(), "TBAI-A99805061-251222-DSI1XA1ErCoqa-");
  QCOMPARE(tbaiInvoice.getIdWithCRC(), "TBAI-A99805061-251222-DSI1XA1ErCoqa-159");
}

void TbaiInvoiceTest::generatesValidUrl()
{
  auto invoice = testInvoice();
  TbaiInvoice tbaiInvoice(*invoice);

  qputenv("TBAI_TAX_AUTHORITY_URL", "https://batuz.eus/QRTBAI/");
  QCOMPARE(
    tbaiInvoice.getUrl().toString(),
    "https://batuz.eus/QRTBAI/?id=TBAI-A99805061-251222-DSI1XA1ErCoqa-159&s=&nf=FAC23&i=7561.00&cr=073"
  );
}
