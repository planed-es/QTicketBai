#include "settings.h"
#include "contactcontroller.h"
#include <QSettings>

Settings::Settings(QObject* parent) : TbaiContext(parent)
{
  QSettings settings("ticketbai-qt", "example");

  connect(this, &Settings::companyChanged, this, &Settings::onCompanyChanged);
  connect(this, &Settings::developerChanged, this, &Settings::onDeveloperChanged);
  setProperty("taxAuthorityUrl", settings.value("settings/taxAuthorityUrl").toUrl());
  setProperty("dumpPath", settings.value("settings/dumpPath").toString());
  setProperty("companyId", settings.value("settings/emitter").toString());
  setProperty("developerId", settings.value("settings/developer").toString());
  software().setLicense(settings.value("settings/software/license").toString());
  software().setCif(settings.value("settings/software/cif").toString());
  software().setName(settings.value("settings/software/name").toString());
  if (!settings.value("settings/software/deviceUid").isNull())
    software().setDeviceUid(settings.value("settings/software/deviceUid").toString().toUtf8());
  certificate().setPassword(settings.value("settings/certificate/password").toString());
  certificate().setPath(settings.value("settings/certificate/path").toString());
}

Settings::~Settings()
{
  save();
}

void Settings::save()
{
  QSettings settings("ticketbai-qt", "example");

  settings.setValue("settings/taxAuthorityUrl",      QVariant::fromValue(taxAuthorityUrl()));
  settings.setValue("settings/dumpPath",             QVariant::fromValue(dumpPath()));
  settings.setValue("settings/software/license",     QVariant::fromValue(software().license()));
  settings.setValue("settings/software/cif",         QVariant::fromValue(software().cif()));
  settings.setValue("settings/software/name",        QVariant::fromValue(software().name()));
  settings.setValue("settings/software/deviceUid",   QVariant::fromValue(software().deviceUid()));
  settings.setValue("settings/certificate/path",     QVariant::fromValue(certificate().path()));
  settings.setValue("settings/certificate/password", QVariant::fromValue(certificate().password()));
  settings.setValue("settings/emitter",              QVariant::fromValue(_companyId));
  settings.setValue("settings/developer",            QVariant::fromValue(_developerId));
}

void Settings::onCompanyChanged()
{
  if (ContactController::self)
  {
    const auto* contact = ContactController::self->find(_companyId);

    if (contact)
      emitter().fromData(contact->asCompanyData());
  }
}

void Settings::onDeveloperChanged()
{
  if (ContactController::self)
  {
    const auto* contact = ContactController::self->find(_developerId);

    if (contact)
    {
      developer().fromData(contact->asCompanyData());
      software().setCif(contact->id());
      software().setName(contact->name());
      emit afterDeveloperChanged();
    }
  }
}
