#ifndef  HELPER_XMLSECTEST_H
# define HELPER_XMLSECTEST_H

# include <QObject>
# include <QTest>
# include "xmlsec-qt/xmlsec.h"
# include "helper_tbai_test.h"

class XmlSecTest : public QTicketBaiTest
{
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();

private:
  QXmlSec xmlsec;
};

#endif
