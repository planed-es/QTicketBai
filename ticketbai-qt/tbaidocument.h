#ifndef  TBAIDOCUMENT_H
# define TBAIDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>

class TbaiInvoiceInterface;

class TbaiDocument : public QDomDocument
{
public:
  TbaiDocument();

  void           createFrom(const TbaiInvoiceInterface&);
  bool           loadFromFile(const QString& path);
  bool           loadFrom(const QByteArray& xml);
  bool           loadFrom(const QString& xml) { return loadFrom(xml.toUtf8()); }
  void           appendSignature(const QDomElement&);
  bool           isSigned() const;
  QByteArray     getSignature() const;

  static QString getFileNameFor(const TbaiInvoiceInterface&);

private:
  QDomElement root;
};

#endif // TBAIDOCUMENT_H
