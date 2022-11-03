#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroesubmitprocess.h"
#include "ticketbai-qt/companydata.h"
#include "ticketbai-qt/invoiceinterface.h"
#include "ticketbai-qt/tbaidocument.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include "ticketbai-qt/uploaddocument.h"
#include "ticketbai-qt/querydocument.h"
#include "xmlsec-qt/xmlsign.h"
#include "helper_invoice.h"
#include <qcurl.h>
#include <iostream>

QTEST_MAIN(LROESubmitProcessTest)

void LROESubmitProcessTest::canSubmitExampleDocument()
{
  CompanyData testEmitter{"5YD5J4IYKM7QJJNDKVAPTFTF6A6QLU", "", "", NifIvaId, "79732487C", "", "", "", ""};
  LROESubmitProcess           lroe(testEmitter);
  LROEDocument                document(LROEDocument::Model140, LROEDocument::AddOperation);
  LROESubmitProcess::Response response;
  QNetworkReply* reply;

  document.setDocumentType(1, 1);
  document.setActivityYear(2022);
  QVERIFY(document.loadFromFile(qgetenv("TBAI_EXAMPLES_PATH") + "/Ejemplo_1_LROE_PF_140_IngresosConFacturaConSG_79732487C.xml"));
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  QCOMPARE(response.status, 200);
}

void LROESubmitProcessTest::canSubmitInvoice()
{
  LROESubmitProcess           lroe;
  TbaiUploadDocument          document(LROEDocument::Model240, LROEDocument::AddOperation);
  TbaiSignProcess             tbai_sign;
  LROESubmitProcess::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  document.setActivityYear(2022);
  document.appendInvoiceFromFile(qgetenv("TBAI_EXAMPLES_PATH") + "/invoice-upload.xml");
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  std::cout << response.document.toString(2).toStdString() << std::endl;
  QCOMPARE(response.status, 200);
}

void LROESubmitProcessTest::canGenerateInvoices()
{
  LROESubmitProcess           lroe;
  TbaiUploadDocument          document(LROEDocument::Model240);
  LROESubmitProcess::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  invoice.m_number = "1";
  invoice.m_name = "Parrot sale";
  auto xml = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice)
  ).xml;
  QVERIFY(xml.length() > 0);
  document.setActivityYear(2022);
  document.appendInvoice(xml);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  std::cout << response.document.toString(2).toStdString() << std::endl;
  QCOMPARE(response.status, 200);
}

void LROESubmitProcessTest::canQueryInvoices()
{
  LROESubmitProcess lroe;
  TbaiQueryDocument document(LROEDocument::Model240);
  LROESubmitProcess::Response response;
  QNetworkReply* reply;

  document.setActivityYear(2022);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  QCOMPARE(response.status, 200);
}
