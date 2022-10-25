#ifndef  TST_LROESUBMITPROCESS_H
# define TST_LROESUBMITPROCESS_H

# include <QObject>
# include <QTest>

class LROESubmitProcessTest : public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();
  void canSubmitExampleDocument();
  void cleanupTestCase();
};

#endif

