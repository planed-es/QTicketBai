#ifndef  TST_TBAISIGNPROCESS_H
# define TST_TBAISIGNPROCESS_H

# include "helper_xmlsec_test.h"

class TbaiSignProcessTest : public XmlSecTest
{
  Q_OBJECT
private slots:
  void signsDocuments();
  void generatesValidDocuments();
};

#endif
