import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
  id: root
  property string modelName
  property QtObject controller

  signal create()
  signal open(QtObject model)

  Label {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: modelName + " Index"
    font.pixelSize: 21
  }

  RowLayout {
    Layout.fillWidth: true

    Button {
      text: "Back"
      onClicked: application.back()
    }

    Button {
      text: "New " + root.modelName
      onClicked: root.create()
    }
  }

  Flickable {
    Layout.fillHeight: true
    Layout.fillWidth: true
    contentWidth: modelList.width
    contentHeight: modelList.height

    ColumnLayout {
      id: modelList
      width: root.width
      Repeater {
        model: root.controller ? root.controller.list : []
        delegate: Rectangle {
          property QtObject model: root.controller ? root.controller.list[index] : null
          property color bgColor: "transparent"
          property color textColor: "black"

          implicitHeight: 50
          border.color: "black"
          border.width: 1
          color: bgColor
          Layout.fillWidth: true

          MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.open(root.controller.list[index])
            onContainsMouseChanged: {
              if (containsMouse) {
                bgColor = "steelblue";
                textColor = "white";
              } else {
                bgColor = "transparent";
                textColor = "black";
              }
            }
          }

          RowLayout {
            spacing: 10
            width: root.width
            anchors.verticalCenter: parent.verticalCenter
            Label { text: model.name; color: textColor; Layout.fillWidth: true }
            Button { text: "Open"; onClicked: root.open(root.controller.list[index]); }
            Button { text: "Remove"; onClicked: root.controller.remove(root.controller.list[index]); }
          }
        }
      }
    }
  }
}


