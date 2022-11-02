#include "tst_tbaiinvoice.h"
#include "ticketbai-qt/tbaiinvoice.h"

QTEST_MAIN(TbaiInvoiceTest)

class TestInvoice : public TbaiInvoiceInterface
{
public:
  TestInvoice* previous = nullptr;
  QByteArray signature, number;
  QString name, description;
  double amount = 0;
  QDateTime date;
  Recipients recipients;

  TbaiInvoiceInterface* getPreviousInvoice() const override { return previous; }
  Type getInvoiceType() const override { return InvoiceType; }
  const QByteArray& getSignature() const override { return signature; }
  const QDateTime& getDate() const override { return date; }
  const QByteArray& getNumber() const override { return number; }
  const QString& getName() const override { return name; }
  const QString& getDescription() const override { return description; }
  double getAmount() const override { return amount; }
  Recipients getRecipients() const override { return recipients; }
  QString getNotSubjectToVatReason() const override { return QString(); }
};

const CompanyData CompanyData::self{
  "5Yd5j4iyKM7qjJnDKVAptFTF6A6qLu",
  "15 camino del caminante",
  "Bilbao",
  "A99805061",
  "+33550209060",
  "+33550209061",
  "12345",
  "e@mail.com"
};

static TestInvoice testInvoice()
{
  TestInvoice invoice;

  invoice.date = QDateTime::fromString("2022-12-25T04:10:24", Qt::ISODate);
  invoice.signature = "DSI1XA1ErCoqaEln++216eTQpN0qF7HQb/I3QhF54V11LEGY3ucMVH33GceRE9LtYJI7AoI0YS3q0qPOn+eTlH6ZHVXXkcVZyi2gBZcuCKqRiRzSiuS0MazDgYqBdrDWhAIByP6AJ2vRhxhZceACeHFNzfmjeDpjX01cUox/RFT//C4cUOuTWADlkq/PqEN0dK/UQbZEOssk4diIlCeYU/exVYoRIpkE9OcFrlwOJv/UJ3ezBlL9pd/6G5rhNMtwqnPXLkbBq993uuVCU2SeKi79HuAa/gCvHlFwrjhwJV6VgcGhPylJ67xJk6dq+ukSW2s6kI4OgauH4SzYUaICAQ==";
  invoice.number = "FAC23";
  invoice.amount = 7561;
  return invoice;
}

void TbaiInvoiceTest::generatesValidTbaiId()
{
  TestInvoice invoice = testInvoice();
  TbaiInvoice tbaiInvoice(&invoice);

  QCOMPARE(tbaiInvoice.getId(), "TBAI-A99805061-251222-DSI1XA1ErCoqa-");
  QCOMPARE(tbaiInvoice.getIdWithCRC(), "TBAI-A99805061-251222-DSI1XA1ErCoqa-159");
}

void TbaiInvoiceTest::generatesValidUrl()
{
  TestInvoice invoice = testInvoice();
  TbaiInvoice tbaiInvoice(&invoice);

  qputenv("TBAI_TAX_AUTHORITY_URL", "https://batuz.eus/QRTBAI/");
  QCOMPARE(
    tbaiInvoice.getUrl().toString(),
    "https://batuz.eus/QRTBAI/?id=TBAI-A99805061-251222-DSI1XA1ErCoqa-159&s=&nf=FAC23&i=7561.00&cr=073"
  );
}
