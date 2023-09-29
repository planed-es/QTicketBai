import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "."

ColumnLayout {
  id: root
  property QtObject model
  property string error
  property bool ready: false

  Label {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Invoice"
    font.pixelSize: 21
  }

  Component.onCompleted: {
    nameInput.text = model.name;
    seriesInput.text = model.series;
    numberInput.text = model.number;
    recipientInput.currentIndex = recipientInput.indexOfValue(model.recipientId);
    ready = true;
  }

  Flickable {
    Layout.fillWidth: true
    Layout.fillHeight: true
    contentHeight: flickableZone.height
    clip: true

    GridLayout {
      id: flickableZone
      columns: 2
      width: root.width

      Label { text: "Name" }
      TextField {
        Layout.fillWidth: true
        id: nameInput
        onTextChanged: model.name = text
        readOnly: model.isSigned
      }

      Label { text: "Series" }
      TextField {
        Layout.fillWidth: true
        id: seriesInput
        onTextChanged: model.series = text
        readOnly: model.isSigned
      }

      Label { text: "Number" }
      TextField {
        Layout.fillWidth: true
        id: numberInput
        onTextChanged: model.number = text;
        readOnly: model.isSigned
      }

      Label { text: "Recipient" }
      ComboBox {
        Layout.fillWidth: true
        id: recipientInput
        textRole: "name"
        valueRole: "id"
        model: contacts.list
        onCurrentIndexChanged: if (ready) { root.model.recipientId = contacts.list[currentIndex].id; } 
        enabled: !model.isSigned
      }

      Label { text: "Lines" }
      Button {
        text: "Add line"
        onClicked: model.addLine()
      }

      Rectangle { Layout.fillWidth: true; Layout.columnSpan: 2; implicitHeight: 2; color: "black" }

      Rectangle {
        Layout.fillWidth: true
        implicitHeight: 20
        RowLayout {
          Label { text: "Label"; Layout.preferredWidth: application.width / 6  }
          Label { text: "Product"; Layout.preferredWidth: application.width / 6 }
          Label { text: "Unit price"; Layout.preferredWidth: application.width / 6 }
          Label { text: "Quantity"; Layout.preferredWidth: application.width / 6 }
          Label { text: "Tax rate"; Layout.preferredWidth: application.width / 6 }
        }
      }

      Repeater {
        model: root.model.lines
        delegate: InvoiceLine {
          Layout.columnSpan: 2
          Layout.fillWidth: true
          model: root.model.lines[index]
          onRequestDeletion: root.model.removeLine(model)
        }
      }

      Rectangle { Layout.fillWidth: true; Layout.columnSpan: 2; implicitHeight: 2; color: "black" }

      Label { text: "Base" }
      Label { text: `${root.model.base}€` }

      Label { text: "Total" }
      Label { text: `${root.model.amount.toFixed(2)}€` }

      Rectangle { Layout.fillWidth: true; Layout.columnSpan: 2; implicitHeight: 2; color: "black" }

      Label { text: "TicketBai URL" }
      Label { text: "<a href=\"" + root.model.tbaiUrl + "\">" + root.model.tbaiUrl + "</a>" }

      Image {
        Layout.columnSpan: 2
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Layout.maximumHeight: 150
        Layout.maximumWidth: 150
        visible: root.model.isSigned
        source: root.model.qrcode
      }

      Label { text: "Signature" }
      TextEdit { text: root.model.signature; selectByMouse: true; readOnly: true }

      Button {
        text: "Sign"
        visible: !root.model.isSigned
        onClicked: {
          if (invoices.sign(root.model))
            root.error = "Successfully signed invoice";
          else
            root.error = "Failed to sign invoice";
        }
      }
    }
  }

  Rectangle {
    Layout.fillWidth: true
    visible: root.error.length > 0
    color: "red"
    implicitHeight: 20
    border.width: 1
    border.color: "yellow"
    Label {
      anchors.centerIn: parent
      text: root.error
      color: "white"
    }
    Timer {
      interval: 5000
      running: root.error.length > 0
      onTriggered: root.error = ""
    }
  }

  Button {
    Layout.fillWidth: true
    text: "Confirm"
    onClicked: application.back()
  }
}
