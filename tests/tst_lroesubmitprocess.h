#ifndef  TST_LROESUBMITPROCESS_H
# define TST_LROESUBMITPROCESS_H

# include "helper_xmlsec_test.h"

class LROESubmitProcessTest : public XmlSecTest
{
  Q_OBJECT
private slots:
  void canSubmitExampleDocument();
  void canSubmitInvoice();
  void canGenerateInvoices();
  void canChainInvoices();
  void canRectifyInvoices();
  void canQueryInvoices();
  void canCancelInvoices();
};

#endif

