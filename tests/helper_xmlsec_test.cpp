#include "helper_xmlsec_test.h"
#include "ticketbai-qt/tbaicertificate.h"

void XmlSecTest::initTestCase()
{
  QTicketBaiTest::initTestCase();
  QVERIFY(ticketbai.context().certificate().prepare());
}

void XmlSecTest::cleanupTestCase()
{
  QTicketBaiTest::cleanupTestCase();
}
