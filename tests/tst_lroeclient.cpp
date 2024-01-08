#include "tst_lroeclient.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroeclient.h"
#include "ticketbai-qt/companydata.h"
#include "ticketbai-qt/tbaiinvoiceinterface.h"
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

QTEST_MAIN(LROEClientTest)

static void storeGeneratedDocument(const QString test, const QString& name, const QDomDocument& document)
{
  static QTemporaryDir directory;
  QString rootDir = directory.path() + '/' + test;
  QFile file(rootDir + '/' + name + ".xml");

  directory.setAutoRemove(false);
  QDir().mkpath(rootDir);
  qDebug() << "Storing debug file" << rootDir << '/' << file.fileName();
  if (file.open(QIODevice::WriteOnly))
    file.write(document.toString(2).toUtf8());
  else
    qDebug() << "Cannot write into " << file.fileName();
}

void LROEClientTest::canGenerateInvoices()
{
  LROEClient           lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice = InvoiceTest::factory("Parrot sale");
  QNetworkReply* reply;
  TbaiDocument tbaiDocument;

  tbaiDocument.createFrom(invoice);
  auto xml = TbaiSignProcess::sign(tbaiDocument).xml;
  QVERIFY(xml.length() > 0);
  document.setActivityYear(invoice.date().date().year());
  document.appendInvoice(xml);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  storeGeneratedDocument("canGenerateInvoices", "LROEDocument", document);
  storeGeneratedDocument("canGenerateInvoices", "TBAIDocument", tbaiDocument);
  storeGeneratedDocument("canGenerateInvoices", "LROEResponse", response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROEClientTest::canSendDocument()
{
  LROEClient           lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice = InvoiceTest::factory("Parrot sale");
  InvoiceTest invoice2 = InvoiceTest::factory("Bird sale", invoice);
  QNetworkReply* reply;
  TbaiDocument tbaiDocumentA, tbaiDocumentB;

  tbaiDocumentA.createFrom(invoice);
  auto result = TbaiSignProcess::sign(tbaiDocumentA);
  invoice.m_signature = result.signature;
  QVERIFY(invoice.signature().length() > 0);
  QVERIFY(result.xml.length() > 0);
  tbaiDocumentB.createFrom(invoice2);
  auto result2 = TbaiSignProcess::sign(tbaiDocumentB);
  invoice2.m_signature = result2.signature;
  QVERIFY(invoice2.signature().length() > 0);
  QVERIFY(result2.xml.length() > 0);
  document.setActivityYear(invoice.date().date().year());
  
  document.appendInvoice(result.xml);
  document.appendInvoice(result2.xml);

  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  storeGeneratedDocument("canChainInvoices", "LROEDocument",  document);
  storeGeneratedDocument("canChainInvoices", "TBAIDocumentA", tbaiDocumentA);
  storeGeneratedDocument("canChainInvoices", "TBAIDocumentB", tbaiDocumentB);
  storeGeneratedDocument("canChainInvoices", "LROEResponse",  response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROEClientTest::canRectifyInvoices()
{
  LROEClient           lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice = InvoiceTest::factory("Parrot sale");
  InvoiceCorrectionTest invoice2 = InvoiceCorrectionTest::factory("Parrot sale correction", invoice);
  QNetworkReply* reply;
  TbaiDocument tbaiDocumentA, tbaiDocumentB;

  invoice2.m_series = "SERIE01";
  invoice2.m_corrected << &invoice;
  tbaiDocumentA.createFrom(invoice);
  auto result = TbaiSignProcess::sign(tbaiDocumentA);
  invoice.m_signature = result.signature;
  QVERIFY(invoice.signature().length() > 0);
  QVERIFY(result.xml.length() > 0);
  tbaiDocumentB.createFrom(invoice2);
  auto result2 = TbaiSignProcess::sign(tbaiDocumentB);
  invoice2.m_signature = result2.signature;
  QVERIFY(invoice2.signature().length() > 0);
  QVERIFY(result2.xml.length() > 0);
  document.setActivityYear(invoice.date().date().year());
  document.appendInvoice(result.xml);
  document.appendInvoice(result2.xml);

  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  storeGeneratedDocument("canRectifyInvoices", "LROEDocument",  document);
  storeGeneratedDocument("canRectifyInvoices", "TBAIDocumentA", tbaiDocumentA);
  storeGeneratedDocument("canRectifyInvoices", "TBAIDocumentB", tbaiDocumentB);
  storeGeneratedDocument("canRectifyInvoices", "LROEResponse",  response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROEClientTest::canQueryInvoices()
{
  LROEClient lroe;
  LROEQueryDocument document(LROEDocument::Model240);
  LROEClient::Response response;
  QNetworkReply* reply;

  document.setActivityYear(2023);
  document.setDocumentType(1, 1);
  document.setSeriesFilter("SERIE01");
  document.setPage(1);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  storeGeneratedDocument("canQueryInvoices", "LROEDocument", document);
  storeGeneratedDocument("canQueryInvoices", "LROEResponse", response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROEClientTest::canCancelInvoices()
{
  LROEClient           lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  InvoiceTest    invoice = InvoiceTest::factory("Parrot sale");
  QNetworkReply* reply;
  TbaiDocument tbaiDocument;

  tbaiDocument.createFrom(invoice);
  auto xml = TbaiSignProcess::sign(tbaiDocument).xml;
  QVERIFY(xml.length() > 0);
  document.setActivityYear(invoice.date().date().year());
  document.appendInvoice(xml);
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");

  LROECancelDocument cancelDocument(LROEDocument::Model240);
  TbaiCancelDocument invoiceDocument;
  invoice.m_number = InvoiceTest::getRandomString();
  invoiceDocument.createFrom(invoice);
  cancelDocument.appendInvoice(invoiceDocument.toString());
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  std::cout << response.document.toString().toStdString() << std::endl;
  storeGeneratedDocument("canCancelInvoices", "LROEDocument",  document);
  storeGeneratedDocument("canCancelInvoices", "TBAIDocument", tbaiDocument);
  storeGeneratedDocument("canCancelInvoices", "TBAICancelDocument", invoiceDocument);
  storeGeneratedDocument("canCancelInvoices", "LROEResponse",  response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}
