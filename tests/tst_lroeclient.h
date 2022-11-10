#ifndef  TST_LROESUBMITPROCESS_H
# define TST_LROESUBMITPROCESS_H

# include "helper_xmlsec_test.h"

class LROEClientTest : public XmlSecTest
{
  Q_OBJECT
private slots:
  void canGenerateInvoices();
  void canChainInvoices();
  void canRectifyInvoices();
  void canQueryInvoices();
  void canCancelInvoices();
};

#endif

