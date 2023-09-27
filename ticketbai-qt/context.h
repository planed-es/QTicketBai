#ifndef  TBAIQT_CONTEXT_H
# define TBAIQT_CONTEXT_H

# include "tbaicontact.h"
# include "tbaicertificate.h"
# include "tbaisoftware.h"
# include <QUrl>

class TICKETBAIQT_EXPORT TbaiContext : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QUrl             taxAuthorityUrl MEMBER _taxAuthorityUrl NOTIFY changed)
  Q_PROPERTY(QString          dumpPath        MEMBER _dumpPath        NOTIFY changed)
  Q_PROPERTY(TbaiCertificate* certificate     READ certificatePtr CONSTANT)
  Q_PROPERTY(TbaiSoftware*    software        READ softwarePtr CONSTANT)
  Q_PROPERTY(TbaiContact*     emitter         READ emitterPtr CONSTANT)
  Q_PROPERTY(TbaiContact*     developer       READ developerPtr CONSTANT)
public:
  TbaiContext(QObject* parent = nullptr);
  virtual ~TbaiContext() {}

  TbaiContext& withEmitter(const CompanyData& value) { _emitter = value; return *this; }
  TbaiContext& withDeveloper(const CompanyData& value) { _developer = value; return *this; }
  TbaiContext& withTaxAuthorityUrl(const QUrl& value) { _taxAuthorityUrl = value; return *this; }
  TbaiContext& withDumpPath(const QString& value) { _dumpPath = value; return *this; }

  TbaiContact& emitter() { return _emitter; }
  TbaiContact& developer() { return _developer; }
  TbaiCertificate& certificate() { return _certificate; }
  TbaiSoftware& software() { return _software; }

  const TbaiContact& constEmitter() const { return _emitter; }
  const TbaiContact& constDeveloper() const { return _developer; }
  const TbaiCertificate& constCertificate() const { return _certificate; }
  const TbaiSoftware& constSoftware() const { return _software; }

  const QUrl&    taxAuthorityUrl() const { return _taxAuthorityUrl; }
  const QString& dumpPath() const { return _dumpPath; }
  void setTaxAuthorityUrl(const QUrl& value) { _taxAuthorityUrl = value; }
  void setDumpPath(const QString& value) { _dumpPath = value; }

signals:
  void changed();

private:
  TbaiCertificate* certificatePtr() { return &_certificate; }
  TbaiSoftware*    softwarePtr() { return &_software; }
  TbaiContact*     emitterPtr() { return &_emitter; }
  TbaiContact*     developerPtr() { return &_developer; }

  TbaiContact     _emitter;
  TbaiContact     _developer;
  TbaiCertificate _certificate;
  TbaiSoftware    _software;
  QUrl            _taxAuthorityUrl;
  QString         _dumpPath;
};
#endif
