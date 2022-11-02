#include <iostream>
#include "tst_tbaisignprocess.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "xmlsec-qt/xmlverify.h"
#include "helper_invoice.h"

QTEST_MAIN(TbaiSignProcessTest)

void TbaiSignProcessTest::signsDocuments()
{
  TbaiSignProcess tbaiSign;
  TbaiDocument document;
  InvoiceTest  invoice;

  invoice.m_number = "FAC01";
  invoice.m_name   = "Parrot sale";
  document.createFrom(invoice);
  QVERIFY(TbaiSignProcess::sign(document));
  QVERIFY(document.isSigned());
}

void TbaiSignProcessTest::generatesValidDocuments()
{
  TbaiSignProcess tbaiSign;
  TbaiDocument document;
  InvoiceTest  invoice;
  QXmlVerify   verifier(TbaiCertificate::sslKey);

  verifier.usePassphrase(TbaiCertificate::password().toUtf8());
  invoice.m_number = "FAC02";
  invoice.m_name   = "Parrot sale";
  document.createFrom(invoice);
  auto signResult = TbaiSignProcess::sign(document);
  QVERIFY(signResult);
  QVERIFY(verifier.verify(signResult.xml));
}
