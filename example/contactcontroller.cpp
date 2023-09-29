#include "contactcontroller.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

ContactController* ContactController::self = nullptr;

ContactController::ContactController(QObject* parent) : QObject(parent)
{
  QSettings settings("ticketbai-qt", "example");
  QJsonDocument document = QJsonDocument::fromJson(settings.value("clients").toString().toUtf8());

  if (self != nullptr)
    throw std::logic_error("ContactController singleton instantiated twice");
  self = this;
  if (document.isArray())
  {
    for (const QJsonValue& value : document.array())
      createContact()->fromString(value.toString());
  }
}

ContactController::~ContactController()
{
  save();
  self = nullptr;
}

void ContactController::save()
{
  QSettings settings("ticketbai-qt", "example");
  QJsonArray jsonList;

  for (const auto* contact : _contacts)
    jsonList << contact->toString();
  settings.setValue("clients", QJsonDocument(jsonList).toJson());
}

const Contact* ContactController::find(const QString& id) const
{
  for (const auto* contact : _contacts)
  {
    if (contact->id() == id)
      return contact;
  }
  return nullptr;
}

Contact* ContactController::createContact()
{
  Contact* contact = new Contact(this);

  _contacts << contact;
  emit listChanged();
  return contact;
}

void ContactController::remove(Contact* contact)
{
  if (contact)
  {
    _contacts.removeAll(contact);
    contact->deleteLater();
    emit listChanged();
  }
}
