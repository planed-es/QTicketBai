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

  LROEDocument(ModelType);

  void initializeIncomeWithInvoices(); // LROE PF 140-240 1.1
  void setOperationId(const QByteArray&);
  void setActivityYear(int year);
  void setDocumentType(int type, int subType);
  bool loadFromFile(const QString& path);

  void appendInvoiceFromFile(const QString& filepath);
  void appendInvoice(const QString&);
  void appendInvoice(const QString&, QDomElement incomeDetails);

  QString documentTypeString() const { return QString::number(documentType) + '.' + QString::number(documentSubtype); }
  ModelType modelType() const { return model; }
  int activityYear() const { return year; }

private:

  ModelType   model;
  QDomElement root;
  QDomElement typeEl, subtypeEl, periodEl, operationEl, emitterEl;
  QDomElement incomeListEl;
  int         documentType, documentSubtype;
  int         year;
};

#endif // LROEDOCUMENT_H
