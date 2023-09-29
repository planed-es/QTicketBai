import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "."

ModelIndex {
  controller: invoices
  modelName: "Invoice"
  onCreate: application.openInvoice(invoices.createInvoice())
  onOpen: application.openInvoice(model)
}
