#include "helper_tbai_test.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include <QDateTime>
#include <random>

void QTicketBaiTest::initTestCase()
{
  ticketbai.certificate()
    .withPath    (qgetenv("TBAI_CERTIFICATE_PATH"))
    .withPassword(qgetenv("TBAI_CERTIFICATE_PASSWORD"))
    .prepare();
  ticketbai.software()
    .withLicense (qgetenv("TBAI_LICENSE"))
    .withCif     (qgetenv("TBAI_SOFTWARE_NIF"))
    .withName    (qgetenv("TBAI_SOFTWARE_NAME"));
  ticketbai
    .withTaxAuthorityUrl(QUrl("https://batuz.eus/QRTBAI/"))
    .withEmitter(CompanyData{
      "5Yd5j4iyKM7qjJnDKVAptFTF6A6qLu",
      "15 camino del caminante",
      "Bilbao",
      "12345",
      TbaiContactDefines::NifIvaId,
      "A99805061",
      "+33550209060",
      "+33550209061",
      "e@mail.com"
    })
    .withDeveloper(CompanyData{
      "QTicketBai Developers",
      "16 camino don camillo",
      "Northrend",
      "54321",
      TbaiContactDefines::PassportId,
      "SOMEPASSPORTID",
      "+33550209061",
      "+33550209062",
      "not@be.ne"
    });
  QVERIFY(TbaiSignProcess::checkSettings(ticketbai));
}

void QTicketBaiTest::cleanupTestCase()
{
}
