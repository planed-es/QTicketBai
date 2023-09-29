import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Application 1.0

Window {
  id: application
  width: 800
  height: 600
  visible: true
  title: "qt-ticketbai-example"

  function back() {
    mainView.pop();
  }

  function openProduct(product) {
    mainView.push("ui/Product.qml", { model: product });
  }

  function openInvoice(invoice) {
    mainView.push("ui/Invoice.qml", { model: invoice });
  }

  function openContact(contact) {
    mainView.push("ui/Contact.qml", { model: contact });
  }

  StackView {
    id: mainView
    initialItem: indexComponent
    anchors.fill: parent
    anchors.margins: 5
  }

  Component {
    id: indexComponent
    Rectangle {
      GridLayout {
        anchors.centerIn: parent
        columns: 2

        Button {
          text: "TicketBAI"
          onClicked: mainView.push("ui/TicketBai.qml")
        }
        Button {
          text: "Contacts"
          onClicked: mainView.push("ui/ContactIndex.qml")
        }
        Button {
          text: "Products"
          onClicked: mainView.push("ui/ProductIndex.qml")
        }
        Button {
          text: "Invoices"
          onClicked: mainView.push("ui/InvoiceIndex.qml")
        }
      }
    }
  }
}
