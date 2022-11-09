#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroesubmitprocess.h"
#include "ticketbai-qt/companydata.h"
#include "ticketbai-qt/invoiceinterface.h"
#include "ticketbai-qt/tbaidocument.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include "ticketbai-qt/uploaddocument.h"
#include "ticketbai-qt/querydocument.h"
#include "ticketbai-qt/canceldocument.h"
#include "xmlsec-qt/xmlsign.h"
#include "helper_invoice.h"
#include <qcurl.h>
#include <iostream>
#include <random>

QTEST_MAIN(LROESubmitProcessTest)

QByteArray getRandomString()
{
 const QByteArray possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
 const int randomStringLength = 12; // assuming you want random strings of 12 characters

 QByteArray randomString;
 for(int i=0; i<randomStringLength; ++i)
 {
   int index = rand() % possibleCharacters.length();
   auto nextChar = possibleCharacters.at(index);
   randomString.push_back(nextChar);
 }
 return randomString;
}

void LROESubmitProcessTest::canSubmitExampleDocument()
{
  CompanyData          testEmitter{"5YD5J4IYKM7QJJNDKVAPTFTF6A6QLU", "", "", NifIvaId, "79732487C", "", "", "", ""};
  LROESubmitProcess    lroe(testEmitter);
  LROEDocument         document(LROEDocument::Model140, LROEDocument::AddOperation);
  LROEClient::Response response;
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
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240, LROEDocument::AddOperation);
  LROEClient::Response response;
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
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  invoice.m_number = getRandomString();
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
  std::cout << "Response:\n";
  std::cout << response.document.toString(2).toStdString() << std::endl;
  std::cout << "Document:\n" << xml.toStdString() << std::endl;
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROESubmitProcessTest::canChainInvoices()
{
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice, invoice2;
  QNetworkReply* reply;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
  invoice2.m_number = getRandomString();
  invoice2.m_name = "Bird sale";
  invoice2.m_previousInvoice = &invoice;
  auto result = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice)
  );
  invoice.m_signature = result.signature;
  QVERIFY(invoice.signature().length() > 0);
  QVERIFY(result.xml.length() > 0);
  auto result2 = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice2)
  );
  invoice2.m_signature = result2.signature;
  QVERIFY(invoice2.signature().length() > 0);
  QVERIFY(result2.xml.length() > 0);
  document.setActivityYear(2022);
  document.appendInvoice(result.xml);
  document.appendInvoice(result2.xml);

  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  std::cout << "Response:\n";
  std::cout << response.document.toString(2).toStdString() << std::endl;
  std::cout << "DocumentA:\n" << result.xml.toStdString() << std::endl;
  std::cout << "DocumentB:\n" << result2.xml.toStdString() << std::endl;
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROESubmitProcessTest::canRectifyInvoices()
{
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  InvoiceCorrectionTest invoice2;
  QNetworkReply* reply;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
  invoice2.m_series = "SERIE01";
  invoice2.m_number = getRandomString();
  invoice2.m_name = "Parrot sale correction";
  invoice2.m_previousInvoice = &invoice;
  invoice2.m_corrected << &invoice;
  auto result = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice)
  );
  invoice.m_signature = result.signature;
  QVERIFY(invoice.signature().length() > 0);
  QVERIFY(result.xml.length() > 0);
  auto result2 = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice2)
  );
  invoice2.m_signature = result2.signature;
  QVERIFY(invoice2.signature().length() > 0);
  QVERIFY(result2.xml.length() > 0);
  document.setActivityYear(2022);
  document.appendInvoice(result.xml);
  document.appendInvoice(result2.xml);

  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROESubmitProcessTest::canQueryInvoices()
{
  LROEClient lroe;
  TbaiQueryDocument document(LROEDocument::Model240);
  LROEClient::Response response;
  QNetworkReply* reply;

  document.setActivityYear(2022);
  document.setDocumentType(1, 1);
  std::cout << "Query document:" << document.toString(2).toStdString() << std::endl;
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROESubmitProcessTest::canCancelInvoices()
{
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  InvoiceTest    invoice;
  QNetworkReply* reply;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
  auto xml = TbaiSignProcess::sign(
    TbaiDocument().createFrom(invoice)
  ).xml;
  QVERIFY(xml.length() > 0);
  document.setActivityYear(2022);
  document.appendInvoice(xml);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");

  LROECancelDocument cancelDocument(LROEDocument::Model240);
  TbaiCancelDocument invoiceDocument;
  invoice.m_number = getRandomString();
  invoiceDocument.createFrom(invoice);
  cancelDocument.appendInvoice(invoiceDocument.toString());
  std::cout << "DEBUG0\n" << invoiceDocument.toString(2).toStdString() << '\n';
  std::cout << "DEBUG1\n" << cancelDocument.toString(2).toStdString() << '\n';
  std::cout << "DEBUG2\n";
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  std::cout << response.document.toString().toStdString() << std::endl;
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}
