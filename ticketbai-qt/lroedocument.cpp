#include "lroedocument.h"
#include "qticketbai.h"
#include <QMap>
#include <QFile>
#include <QDebug>

static QByteArray xmlScope = "lrpjfecsgap";
const QByteArray LROEDocument::apiVersion = "1.0";

static QMap<
  QPair<LROEDocument::ModelType, LROEDocument::OperationType>,
  QString
> LROETagTypes = {
  { {LROEDocument::Model140, LROEDocument::AddOperation},    "LROEPF140IngresosConFacturaConSGAltaPeticion"},
  { {LROEDocument::Model240, LROEDocument::AddOperation},    "LROEPJ240FacturasEmitidasConSGAltaPeticion"},
  { {LROEDocument::Model140, LROEDocument::CancelOperation}, "LROEPF140IngresosConFacturaConSGAnulacionPeticion"},
  { {LROEDocument::Model240, LROEDocument::CancelOperation}, "LROEPJ240FacturasEmitidasConSGAnulacionPeticion"},
  { {LROEDocument::Model140, LROEDocument::QueryOperation},  "LROEPF140IngresosConFacturaConSGConsultaPeticion"},
  { {LROEDocument::Model240, LROEDocument::QueryOperation},  "LROEPJ240FacturasEmitidasConSGConsultaPeticion"}
};

static QMap<
  QPair<LROEDocument::ModelType, LROEDocument::OperationType>,
  QString
> LROEXmlns = {
  { {LROEDocument::Model140, LROEDocument::AddOperation},    "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PF_140_1_1_Ingresos_ConfacturaConSG_AltaPeticion_V1_0_2.xsd"},
  { {LROEDocument::Model240, LROEDocument::AddOperation},    "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PJ_240_1_1_FacturasEmitidas_ConSG_AltaPeticion_V1_0_2.xsd"},
  { {LROEDocument::Model140, LROEDocument::CancelOperation}, "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PF_140_1_1_Ingresos_ConfacturaConSG_AnulacionPeticion_V1_0_0.xsd"},
  { {LROEDocument::Model240, LROEDocument::CancelOperation}, "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PJ_240_1_1_FacturasEmitidas_ConSG_AnulacionPeticion_V1_0_0.xsd"},
  { {LROEDocument::Model140, LROEDocument::QueryOperation},  "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PF_140_1_1_Ingresos_ConfacturaConSG_ConsultaPeticion_V1_0_0.xsd"},
  { {LROEDocument::Model240, LROEDocument::QueryOperation},  "https://www.batuz.eus/fitxategiak/batuz/LROE/esquemas/LROE_PJ_240_1_1_FacturasEmitidas_ConSG_ConsultaPeticion_V1_0_0.xsd"}
};

static QMap<LROEDocument::OperationType, QString> LROEOperationTypes = {
  {LROEDocument::AddOperation,    "A00"},
  {LROEDocument::ModifyOperation, "M00"},
  {LROEDocument::CancelOperation, "AN0"},
  {LROEDocument::QueryOperation,  "C00"}
};

static QDomElement generateEmitterXml(QDomDocument& document, const TbaiContext& context)
{
  QDomElement root   = document.createElement("ObligadoTributario");
  QDomElement cifEl  = document.createElement("NIF");
  QDomElement nameEl = document.createElement("ApellidosNombreRazonSocial");

  cifEl.appendChild(document.createTextNode(context.constEmitter().id()));
  nameEl.appendChild(document.createTextNode(context.constEmitter().name()));
  root.appendChild(cifEl);
  root.appendChild(nameEl);
  return root;
}

LROEDocument::LROEDocument(const TbaiContext& context, ModelType modelType, OperationType operationType)
{
  initialize(context, modelType, operationType);
}

LROEDocument::LROEDocument(ModelType modelType, OperationType operationType)
{
  initialize(QTicketBai::context(), modelType, operationType);
}

void LROEDocument::initialize(const TbaiContext& context, ModelType modelType, OperationType operationType)
{
  QDomElement header    = createElement("Cabecera");
  QDomElement modelEl   = createElement("Modelo");
  QDomElement versionEl = createElement("Version");
  auto xmlns = LROEXmlns.find({modelType, operationType});

  model       = modelType;
  operation   = operationType;
  root        = createElement(xmlScope + ':' + LROETagTypes[{modelType, operationType}]);
  typeEl      = createElement("Capitulo");
  subtypeEl   = createElement("Subcapitulo");
  emitterEl   = createElement("ObligadoTributario");
  periodEl    = createElement("Ejercicio");
  operationEl = createElement("Operacion");
  if (xmlns != LROEXmlns.end())
    root.setAttribute("xmlns:" + xmlScope, xmlns.value());
  modelEl.appendChild(createTextNode(QString::number(static_cast<int>(modelType))));
  operationEl.appendChild(createTextNode(LROEOperationTypes[operationType]));
  versionEl.appendChild(createTextNode(apiVersion));
  header.appendChild(modelEl);
  header.appendChild(typeEl);
  header.appendChild(subtypeEl);
  header.appendChild(operationEl);
  header.appendChild(versionEl);
  header.appendChild(periodEl);
  header.appendChild(generateEmitterXml(*this, context));
  root.appendChild(header);
  appendChild(root);
}

void resetElement(QDomElement& el)
{
  while (!el.lastChild().isNull())
    el.removeChild(el.lastChild());
}

void LROEDocument::setDocumentType(int type, int subType)
{
  documentType    = type;
  documentSubtype = subType;
  resetElement(typeEl);
  resetElement(subtypeEl);
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
