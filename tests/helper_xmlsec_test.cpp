#include "helper_xmlsec_test.h"
#include "ticketbai-qt/tbaicertificate.h"

void XmlSecTest::initTestCase()
{
  QVERIFY(TbaiCertificate::prepare());
}

void XmlSecTest::cleanupTestCase()
{
  TbaiCertificate::cleanup();
}
