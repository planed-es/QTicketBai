#ifndef  TBAIDOCUMENT_H
# define TBAIDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>

class TbaiInvoiceInterface;

class TbaiDocument : public QDomDocument
{
public:
  TbaiDocument();

  void createFrom(const TbaiInvoiceInterface&);
  void appendSignature(const QDomElement&);

  static QString getFileNameFor(const TbaiInvoiceInterface&);

private:
  QDomElement root;
};

#endif // TBAIDOCUMENT_H
