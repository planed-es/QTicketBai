#include "tst_tbaicertificate.h"
#include "ticketbai-qt/tbaicertificate.h"

QTEST_MAIN(TbaiCertificateTest)

void TbaiCertificateTest::generatesPemKeyAndCertificateFromPkcs12()
{
  TbaiCertificate tbaiCertificate;

  QCOMPARE(tbaiCertificate.pemCertificatePath(), QString());
  QCOMPARE(tbaiCertificate.pemKeyPath(), QString());
  tbaiCertificate.setPassword(qgetenv("TBAI_CERTIFICATE_PASSWORD"));
  tbaiCertificate.setPath(qgetenv("TBAI_CERTIFICATE_PATH"));
  {
    QFileInfo pemInfo(tbaiCertificate.pemCertificatePath());
    QFileInfo keyInfo(tbaiCertificate.pemKeyPath());

    QVERIFY(pemInfo.exists());
    QVERIFY(keyInfo.exists());

    QCOMPARE(pemInfo.permission(QFileDevice::WriteUser), true);
    QCOMPARE(pemInfo.permission(QFileDevice::ReadUser), true);
    QCOMPARE(pemInfo.permission(QFileDevice::ExeUser), false);
    QCOMPARE(pemInfo.permission(QFileDevice::WriteGroup), false);
    QCOMPARE(pemInfo.permission(QFileDevice::ReadGroup), false);
    QCOMPARE(pemInfo.permission(QFileDevice::ExeGroup), false);
    QCOMPARE(pemInfo.permission(QFileDevice::WriteOther), false);
    QCOMPARE(pemInfo.permission(QFileDevice::ReadOther), false);
    QCOMPARE(pemInfo.permission(QFileDevice::ExeOther), false);

    QCOMPARE(keyInfo.permission(QFileDevice::WriteUser), true);
    QCOMPARE(keyInfo.permission(QFileDevice::ReadUser), true);
    QCOMPARE(keyInfo.permission(QFileDevice::ExeUser), false);
    QCOMPARE(keyInfo.permission(QFileDevice::WriteGroup), false);
    QCOMPARE(keyInfo.permission(QFileDevice::ReadGroup), false);
    QCOMPARE(keyInfo.permission(QFileDevice::ExeGroup), false);
    QCOMPARE(keyInfo.permission(QFileDevice::WriteOther), false);
    QCOMPARE(keyInfo.permission(QFileDevice::ReadOther), false);
    QCOMPARE(keyInfo.permission(QFileDevice::ExeOther), false);
  }
}
