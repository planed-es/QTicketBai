#include "helper_tbai_test.h"

void QTicketBaiTest::initTestCase()
{
  ticketbai
    .withUser(CompanyData{
      "5Yd5j4iyKM7qjJnDKVAptFTF6A6qLu",
      "15 camino del caminante",
      "Bilbao",
      NifIvaId,
      "A99805061",
      "+33550209060",
      "+33550209061",
      "12345",
      "e@mail.com"
    });
  ticketbai
    .withDeveloper(CompanyData{
      "QTicketBai Developers",
      "16 camino don camillo",
      "Northrend",
      PassportId,
      "SOMEPASSPORTID",
      "+33550209061",
      "+33550209062",
      "54321",
      "not@be.ne"
    });
}

void QTicketBaiTest::cleanupTestCase()
{
}
