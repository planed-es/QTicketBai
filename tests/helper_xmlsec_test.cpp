#include "helper_xmlsec_test.h"
#include "ticketbai-qt/tbaicertificate.h"

void XmlSecTest::initTestCase()
{
  QTicketBaiTest::initTestCase();
  QVERIFY(TbaiCertificate::prepare());
}

void XmlSecTest::cleanupTestCase()
{
  QTicketBaiTest::cleanupTestCase();
  TbaiCertificate::cleanup();
}
