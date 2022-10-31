#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroesubmitprocess.h"
#include "ticketbai-qt/companydata.h"
#include "ticketbai-qt/invoiceinterface.h"
#include "ticketbai-qt/tbaidocument.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include "ticketbai-qt/uploaddocument.h"
#include "ticketbai-qt/querydocument.h"
#include "xmlsec-qt/xmlsec.h"
#include "xmlsec-qt/xmlsign.h"
#include <qcurl.h>
#include <iostream>

QTEST_MAIN(LROESubmitProcessTest)

const CompanyData CompanyData::self{
  "5Yd5j4iyKM7qjJnDKVAptFTF6A6qLu",
  "15 camino del caminante",
  "Bilbao",
  "A99805061",
  "+33550209060",
  "+33550209061",
  "12345",
  "e@mail.com"
};

QXmlSec* xmlsec;

void LROESubmitProcessTest::initTestCase()
{
  xmlsec = new QXmlSec();
  QVERIFY(TbaiCertificate::prepare());
}

void LROESubmitProcessTest::cleanupTestCase()
{
  delete xmlsec;
  TbaiCertificate::cleanup();
}

void LROESubmitProcessTest::canSubmitExampleDocument()
{
  CompanyData testEmitter{"5YD5J4IYKM7QJJNDKVAPTFTF6A6QLU", "", "", "79732487C", "", "", "", ""};
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

class InvoiceTest : public TbaiInvoiceInterface
{
public:
  QDateTime date = QDateTime::currentDateTime();
  QByteArray signature, series, number;
  QString name;

  TbaiInvoiceInterface* getPreviousInvoice() const override { return nullptr; }
  Type getInvoiceType() const override { return InvoiceType; }
  const QByteArray& getSignature() const override { return signature; }
  const QDateTime&  getDate() const override { return date; }
  const QByteArray& getNumber() const override { return number; }
  const QString&    getName() const override { return name; }
  const QString&    getDescription() const override { return name; }
  double            getAmount() const override { return 42.42; }
  QString           getNotSubjectToVatReason() const override { return ""; }

  Recipients        getRecipients() const override
  {
    return {
      CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", "A12345678", "0505050505", "0606060606", "12345", "e@ma.il"}
    };
  }
};

void LROESubmitProcessTest::canSubmitInvoice()
{
  LROESubmitProcess           lroe;
  TbaiUploadDocument       document(LROEDocument::Model240, LROEDocument::AddOperation);
  TbaiSignProcess             tbai_sign;
  LROESubmitProcess::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  document.setActivityYear(2022);
  document.appendInvoiceFromFile(qgetenv("TBAI_EXAMPLES_PATH") + "/invoice-upload.xml");
  //std::cout << "DEBUG TBAI:\n" << invoiceXml.toStdString() << "\nEND DEBUG TBAI DOC" << std::endl;
  //std::cout << "DEBUG DOC:\n" << document.toString(2).toStdString() << "\nEND DEBUG DOC" << std::endl;
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
  TbaiSignProcess             tbai_sign;
  LROESubmitProcess::Response response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  invoice.number = "1";
  invoice.name = "Parrot sale";
  QObject::connect(&tbai_sign, &TbaiSignProcess::generatedSignature, [](QByteArray signature)
  {
    qDebug() << "Generated signature" << signature;
  });
  QObject::connect(&tbai_sign, &TbaiSignProcess::generatedXml, [&invoiceXml](QByteArray generatedXml)
  {
    invoiceXml = generatedXml;
    std::cout << "InvoiceXML = " << invoiceXml.toStdString() << std::endl;
  });
  tbai_sign.sign(invoice);
  QVERIFY(invoiceXml.length() > 0);
  document.setActivityYear(2022);
  document.appendInvoice(invoiceXml);
  //std::cout << "DEBUG TBAI:\n" << invoiceXml.toStdString() << "\nEND DEBUG TBAI DOC" << std::endl;
  //std::cout << "DEBUG DOC:\n" << document.toString(2).toStdString() << "\nEND DEBUG DOC" << std::endl;
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
  std::cout << "DEBUG DOC:\n" << document.toString(2).toStdString() << std::endl;
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  qDebug() << response;
  QCOMPARE(response.status, 200);
}
