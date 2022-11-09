#ifndef  TBAI_UPLOAD_DOCUMENT_HPP
# define TBAI_UPLOAD_DOCUMENT_HPP

# include "lroedocument.h"

class TICKETBAIQT_EXPORT LROEUploadDocument : public LROEDocument
{
public:
  LROEUploadDocument(ModelType, OperationType = AddOperation);

  void appendInvoiceFromFile(const QString& filepath);
  void appendInvoice(const QString& invoiceXml);

private:
  QDomElement appendRevenue(const QString& invoiceXml);

  QDomElement incomeListEl;
};

#endif
