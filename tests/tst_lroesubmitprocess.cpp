#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroesubmitprocess.h"
#include "ticketbai-qt/companydata.h"
#include <qcurl.h>
#include "ticketbai-qt/qcompressor.h"

QTEST_MAIN(LROESubmitProcessTest)

const CompanyData CompanyData::self;

void LROESubmitProcessTest::initTestCase()
{
  QVERIFY(TbaiCertificate::prepare());
}

void LROESubmitProcessTest::cleanupTestCase()
{
  TbaiCertificate::cleanup();
}

static void logLROEResponse(const LROESubmitProcess::LROEResponse& response)
{
  qDebug() << "LROE Remote Server responded with:";
  qDebug() << "  Status: " << response.status;
  qDebug() << "  Type:"    << response.type;
  qDebug() << "  Code:"    << response.code;
  qDebug() << "  Msg :"    << response.message;
  qDebug() << "  Id  :"    << response.id;
  qDebug() << "  Document:";
  qDebug() << response.document.toByteArray(2) << "\n";
}

void LROESubmitProcessTest::canSubmitExampleDocument()
{
  LROESubmitProcess               lroe;
  LROEDocument                    document(LROEDocument::Model140);
  LROESubmitProcess::LROEResponse response;
  QNetworkReply* reply;

  document.setDocumentType(1, 1);
  document.setActivityYear(2022);
  QVERIFY(document.loadFromFile("Ejemplo_1_LROE_PF_140_IngresosConFacturaConSG_79732487C.xml"));
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  logLROEResponse(response);
  QCOMPARE(response.status, 200);
}
