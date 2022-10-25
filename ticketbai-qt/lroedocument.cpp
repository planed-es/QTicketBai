#include "lroedocument.h"
#include "companydata.h"
#include <QMap>
#include <QFile>
#include <QDebug>

static QByteArray xmlScope = "lrpjfecsgap";
const QByteArray LROEDocument::apiVersion = "1.0";

static QMap<LROEDocument::ModelType, QString> LROETagTypes = {
  {LROEDocument::Model140, "LROEPF140IngresosConFacturaConSGAltaPeticion"},
  {LROEDocument::Model240, "LROEPJ240FacturasEmitidasConSGAltaPeticion"}
};

static QMap<LROEDocument::ModelType, QString> LROEXmlns = {
  {LROEDocument::Model140, "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PF_140_1_1_Ingresos_ConfacturaConSG_AltaPeticion_V1_0_2.xsd"},
  {LROEDocument::Model240, "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PJ_240_1_1_FacturasEmitidas_ConSG_AltaPeticion_V1_0_2.xsd"}
};

static QDomElement generateEmitterXml(QDomDocument& document)
{
  QDomElement root   = document.createElement("ObligadoTributario");
  QDomElement cifEl  = document.createElement("NIF");
  QDomElement nameEl = document.createElement("ApellidosNombreRazonSocial");

  cifEl.appendChild(document.createTextNode(CompanyData::self.cif));
  nameEl.appendChild(document.createTextNode(CompanyData::self.name));
  root.appendChild(cifEl);
  root.appendChild(nameEl);
  return root;
}

LROEDocument::LROEDocument(ModelType modelType)
{
  QDomElement header    = createElement("Cabecera");
  QDomElement modelEl   = createElement("Modelo");
  QDomElement versionEl = createElement("Version");

  model       = modelType;
  root        = createElement(xmlScope + ':' + LROETagTypes[modelType]);
  typeEl      = createElement("Capitulo");
  subtypeEl   = createElement("Subcapitulo");
  emitterEl   = createElement("ObligadoTributario");
  periodEl    = createElement("Ejercicio");
  operationEl = createElement("Operacion");
  modelEl.appendChild(createTextNode(QString::number(static_cast<int>(modelType))));
  root.attribute("xmlns:" + xmlScope, LROEXmlns[modelType]);
  versionEl.appendChild(createTextNode(apiVersion));
  header.appendChild(modelEl);
  header.appendChild(typeEl);
  header.appendChild(subtypeEl);
  header.appendChild(operationEl);
  header.appendChild(versionEl);
  header.appendChild(periodEl);
  header.appendChild(generateEmitterXml(*this));
  root.appendChild(header);
  appendChild(root);
}

void LROEDocument::initializeIncomeWithInvoices()
{
  setDocumentType(1, 1);
  incomeListEl = createElement("Ingresos");
  root.appendChild(incomeListEl);
}

void LROEDocument::setDocumentType(int type, int subType)
{
  documentType    = type;
  documentSubtype = subType;
  typeEl.appendChild(createTextNode(QString::number(type)));
  subtypeEl.appendChild(createTextNode(documentTypeString()));
}

void LROEDocument::setOperationId(const QByteArray& id)
{
  operationEl.appendChild(createTextNode(id));
}

void LROEDocument::setActivityYear(int value)
{
  year = value;
  periodEl.appendChild(createTextNode(QString::number(year)));
}

void LROEDocument::appendInvoiceFromFile(const QString& filepath)
{
  QFile file(filepath);

  if (file.open(QIODevice::ReadOnly))
    appendInvoice(file.readAll());
  else
    throw std::runtime_error("LROEDocument::appendInvoiceFromFile: cannot open " + filepath.toStdString());
}

void LROEDocument::appendInvoice(const QString& invoice)
{
  QDomElement incomeDetailsEl = createElement("DetalleRenta");
  QDomElement epigraphEl      = createElement("Epigrafe");

  epigraphEl.appendChild(createTextNode("197330"));
  incomeDetailsEl.appendChild(epigraphEl);
  appendInvoice(invoice, incomeDetailsEl);
}

void LROEDocument::appendInvoice(const QString& invoice, QDomElement incomeDetailsEl)
{
  QDomElement invoiceEl       = createElement("Ingreso");
  QDomElement tbaiEl          = createElement("TicketBai");
  QDomElement incomeEl        = createElement("Renta");

  tbaiEl.appendChild(createTextNode(invoice.toUtf8().toBase64()));
  incomeEl.appendChild(incomeDetailsEl);
  invoiceEl.appendChild(tbaiEl);
  invoiceEl.appendChild(incomeEl);
  incomeListEl.appendChild(invoiceEl);
}

bool LROEDocument::loadFromFile(const QString& filepath)
{
  QFile file(filepath);

  if (file.open(QIODevice::ReadOnly))
  {
    setContent(&file);
    return true;
  }
  return false;
}
