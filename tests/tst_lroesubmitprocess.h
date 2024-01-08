#ifndef  TST_LROESUBMITPROCESS_H
# define TST_LROESUBMITPROCESS_H

# include "helper_xmlsec_test.h"

class LROESubmitProcessTest : public XmlSecTest
{
  Q_OBJECT
private slots:
  void canSubmitInvoicesFromDumpPath();
};

#endif
