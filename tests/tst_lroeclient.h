#ifndef  TST_LROECLIENT_H
# define TST_LROECLIENT_H

# include "helper_xmlsec_test.h"

class LROEClientTest : public XmlSecTest
{
  Q_OBJECT
private slots:
  void canGenerateInvoices();
  void canSendDocument();
  void canRectifyInvoices();
  void canQueryInvoices();
  void canCancelInvoices();
};

#endif

