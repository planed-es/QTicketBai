import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "."

ModelIndex {
  controller: contacts
  modelName: "Contact"
  onCreate: application.openContact(contacts.createContact())
  onOpen: application.openContact(model)
}

