#ifndef  HELPER_QTICKETBAI_TEST_H
# define HELPER_QTICKETBAI_TEST_H

# include <QObject>
# include <QTest>
# include "ticketbai-qt/qticketbai.h"

class QTicketBaiTest : public QObject
{
  Q_OBJECT
protected slots:
  void initTestCase();
  void cleanupTestCase();
protected:
  QTicketBai ticketbai;
};

#endif
