#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/tbaicertificate.h"
#include "ticketbai-qt/lroesubmitprocess.h"
#include "ticketbai-qt/companydata.h"
#include "ticketbai-qt/invoiceinterface.h"
#include "ticketbai-qt/tbaidocument.h"
#include "ticketbai-qt/tbaisignprocess.h"
#include "ticketbai-qt/invoiceuploaddocument.h"
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

extern QString testCif;
extern QString testCompanyName;

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
  LROEDocument                    document(LROEDocument::Model140, LROEDocument::AddOperation);
  LROESubmitProcess::LROEResponse response;
  QNetworkReply* reply;

  testCif = "79732487C";
  testCompanyName = "5YD5J4IYKM7QJJNDKVAPTFTF6A6QLU";
  document.setDocumentType(1, 1);
  document.setActivityYear(2022);
  QVERIFY(document.loadFromFile(qgetenv("TBAI_EXAMPLES_PATH") + "/Ejemplo_1_LROE_PF_140_IngresosConFacturaConSG_79732487C.xml"));
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  logLROEResponse(response);
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
      CompanyData{"Francis Huster", "1 Rue de la paix", "Imagination Land", "0505050505", "0606060606", "12345", "e@ma.il"}
    };
  }
};

void LROESubmitProcessTest::canSubmitInvoice()
{
  LROESubmitProcess               lroe;
  InvoiceUploadDocument           document(LROEDocument::Model240, LROEDocument::AddOperation);
  TbaiSignProcess                 tbai_sign;
  LROESubmitProcess::LROEResponse response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  testCif = CompanyData::self.cif;
  testCompanyName = CompanyData::self.name;
  document.setActivityYear(2022);
  document.appendInvoiceFromFile(qgetenv("TBAI_EXAMPLES_PATH") + "/invoice-upload.xml");
  //std::cout << "DEBUG TBAI:\n" << invoiceXml.toStdString() << "\nEND DEBUG TBAI DOC" << std::endl;
  //std::cout << "DEBUG DOC:\n" << document.toString(2).toStdString() << "\nEND DEBUG DOC" << std::endl;
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  logLROEResponse(response);
  QCOMPARE(response.status, 200);
}

void LROESubmitProcessTest::canGenerateInvoices()
{
  LROESubmitProcess               lroe;
  InvoiceUploadDocument           document(LROEDocument::Model240, LROEDocument::AddOperation);
  TbaiSignProcess                 tbai_sign;
  LROESubmitProcess::LROEResponse response;
  QString invoiceXml;
  InvoiceTest invoice;
  QNetworkReply* reply;

  testCif = CompanyData::self.cif;
  testCompanyName = CompanyData::self.name;
  invoice.number = "1";
  invoice.name = "Parrot sale";
  QObject::connect(&tbai_sign, &TbaiSignProcess::generatedXml, [&invoiceXml](QString xml)
  {
    invoiceXml = xml;
  });
  tbai_sign.sign(invoice);
  QVERIFY(tbai_sign.wait());
  document.setActivityYear(2022);
  document.appendInvoice(invoiceXml);
  //std::cout << "DEBUG TBAI:\n" << invoiceXml.toStdString() << "\nEND DEBUG TBAI DOC" << std::endl;
  //std::cout << "DEBUG DOC:\n" << document.toString(2).toStdString() << "\nEND DEBUG DOC" << std::endl;
  reply = lroe.sendDocument(document);
  response = lroe.parseResponse(reply);
  logLROEResponse(response);
  QCOMPARE(response.status, 200);
}

