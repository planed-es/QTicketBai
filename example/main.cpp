#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QMetaType>
#include <ticketbai-qt/tbaisignprocess.h>
#include <xmlsec-qt/xmlsec.h>
#include "invoicecontroller.h"
#include "productcontroller.h"
#include "contactcontroller.h"
#include "settings.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  QCoreApplication::setOrganizationName("PlanED");
  QCoreApplication::setOrganizationDomain("planed.es");
  QCoreApplication::setApplicationName("QTicketBai Example");
  QXmlSec           xmlsec;
  ContactController contacts;
  ProductController products;
  Settings          ticketBai;
  InvoiceController invoices(ticketBai);

  qmlRegisterType<InvoiceLine>("Application", 1,0, "InvoiceLine");
  qmlRegisterType<Invoice>("Application", 1,0, "Invoice");
  qmlRegisterType<Product>("Application", 1,0, "Product");
  qmlRegisterType<Contact>("Application", 1,0, "Contact");
  qmlRegisterType<TbaiInvoiceDefines>("Application", 1,0, "InvoiceDefines");
  qmlRegisterType<TbaiContactDefines>("Application", 1,0, "ContactDefines");

  qRegisterMetaType<TbaiCertificate*>("TbaiCertificate*");
  qRegisterMetaType<TbaiSoftware*>("TbaiSoftware*");

  engine.rootContext()->setContextProperty("invoices", &invoices);
  engine.rootContext()->setContextProperty("products", &products);
  engine.rootContext()->setContextProperty("contacts", &contacts);
  engine.rootContext()->setContextProperty("ticketBai", &ticketBai);

  QObject::connect(&engine,
    &QQmlApplicationEngine::objectCreationFailed,
    &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);

  engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
  return app.exec();
}

