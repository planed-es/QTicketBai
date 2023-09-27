#include "tst_lroeclient.h"
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

QTEST_MAIN(LROEClientTest)

QByteArray getRandomString()
{
 const QByteArray possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
 const int randomStringLength = 12;

 QByteArray randomString;
 for(int i=0; i<randomStringLength; ++i)
 {
   int index = rand() % possibleCharacters.length();
   auto nextChar = possibleCharacters.at(index);
   randomString.push_back(nextChar);
 }
 return randomString;
}

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
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;
  TbaiDocument tbaiDocument;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
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

void LROEClientTest::canChainInvoices()
{
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice, invoice2;
  QNetworkReply* reply;
  TbaiDocument tbaiDocumentA, tbaiDocumentB;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
  invoice2.m_number = getRandomString();
  invoice2.m_name = "Bird sale";
  invoice2.m_previousInvoice = &invoice;
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
  qDebug() << response;
  storeGeneratedDocument("canChainInvoices", "LROEDocument",  document);
  storeGeneratedDocument("canChainInvoices", "TBAIDocumentA", tbaiDocumentA);
  storeGeneratedDocument("canChainInvoices", "TBAIDocumentB", tbaiDocumentB);
  storeGeneratedDocument("canChainInvoices", "LROEResponse",  response.document);
  QCOMPARE(response.status, 200);
  QCOMPARE(response.type, "Correcto");
}

void LROEClientTest::canRectifyInvoices()
{
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  InvoiceCorrectionTest invoice2;
  QNetworkReply* reply;
  TbaiDocument tbaiDocumentA, tbaiDocumentB;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
  invoice2.m_series = "SERIE01";
  invoice2.m_number = getRandomString();
  invoice2.m_name = "Parrot sale correction";
  invoice2.m_previousInvoice = &invoice;
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
  LROESubmitProcess    lroe;
  LROEUploadDocument   document(LROEDocument::Model240);
  LROEClient::Response response;
  InvoiceTest    invoice;
  QNetworkReply* reply;
  TbaiDocument tbaiDocument;

  invoice.m_number = getRandomString();
  invoice.m_name = "Parrot sale";
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
  invoice.m_number = getRandomString();
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
