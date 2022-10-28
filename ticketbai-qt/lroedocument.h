#ifndef  LROEDOCUMENT_H
# define LROEDOCUMENT_H

# include <QDomDocument>

class LROEDocument : public QDomDocument
{
public:
  static const QByteArray apiVersion;

  enum ModelType
  {
    Model140 = 140,
    Model240 = 240
  };

  enum OperationType
  {
    AddOperation,
    ModifyOperation,
    CancelOperation,
    QueryOperation
  };

  LROEDocument(ModelType, OperationType);

  void setOperationId(const QByteArray&);
  void setActivityYear(int year);
  void setDocumentType(int type, int subType);
  bool loadFromFile(const QString& path);

  QString documentTypeString() const { return QString::number(documentType) + '.' + QString::number(documentSubtype); }
  ModelType modelType() const { return model; }
  int activityYear() const { return year; }

protected:
  ModelType   model;
  QDomElement root;
  QDomElement typeEl, subtypeEl, periodEl, operationEl, emitterEl;
  int         documentType, documentSubtype;
  int         year;
};

#endif // LROEDOCUMENT_H
