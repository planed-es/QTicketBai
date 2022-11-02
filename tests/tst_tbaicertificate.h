#ifndef  TST_TBAICERTIFICATE_H
# define TST_TBAICERTIFICATE_H

# include <QObject>
# include <QTest>

class TbaiCertificateTest : public QObject
{
  Q_OBJECT
private slots:
  void generatesPemKeyAndCertificateFromPkcs12();
};

#endif
