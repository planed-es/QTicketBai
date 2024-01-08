#include "tst_lroesubmitprocess.h"
#include "ticketbai-qt/lroesubmitprocess.h"

QTEST_MAIN(LROESubmitProcessTest)

void LROESubmitProcessTest::canSubmitInvoicesFromDumpPath()
{
  LROESubmitProcess lroe;
  bool errorOccuredCalled = false;
  bool finishedCalled = false;

  connect(&lroe, &LROESubmitProcess::errorOccured, this, [&](LROESubmitProcess::Response)
  {
    errorOccuredCalled = true;
  });
  connect(&lroe, &LROESubmitProcess::finished, this, [&]()
  {
    finishedCalled = true;
  });

  lroe.submitAll();
  QVERIFY(!errorOccuredCalled);
  QVERIFY(finishedCalled);
  QCOMPARE(lroe.pendingFiles().size(), 0);
}
