#ifndef  INVOICE_UPLOAD_DOCUMENT_HPP
# define INVOICE_UPLOAD_DOCUMENT_HPP

# include "lroedocument.h"

class InvoiceUploadDocument : public LROEDocument
{
public:
  InvoiceUploadDocument(ModelType, OperationType);

  void appendInvoiceFromFile(const QString& filepath);
  void appendInvoice(const QString& invoiceXml);

private:
  QDomElement incomeListEl;
};

#endif
