import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
  property QtObject model: ticketBai
  property bool ready: false

  columns: 2
  columnSpacing: 5
  rowSpacing: 5

  Component.onCompleted: {
    taxAuthorityUrl.text = model.taxAuthorityUrl.toString();
    dumpPath.text = model.dumpPath;
    certificatePath.text = model.certificate.path;
    certificatePassword.text = model.certificate.password;
    license.text = model.software.license;
    developerCif.text = model.software.cif;
    developerName.text = model.software.name;
    deviceUid.text = model.software.deviceUid;
    company.currentIndex = company.indexOfValue(model.companyId);
    developer.currentIndex = developer.indexOfValue(model.developerId);
    ready = true;
  }

  Connections {
    target: ticketBai
    function onAfterDeveloperChanged() {
      developerCif.text = model.software.cif;
      developerName.text = model.software.name;
    }
  }

  Label {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "TicketBAI Settings"
    font.pixelSize: 21
  }

  Label { text: "Tax Authority URL" }
  TextField {
    Layout.fillWidth: true
    id: taxAuthorityUrl
    onTextChanged: model.taxAuthorityUrl = text
  }

  Label { text: "Document directory" }
  TextField {
    Layout.fillWidth: true
    id: dumpPath
    onTextChanged: model.dumpPath = text;
  }

  Label { text: "Certificate path" }
  TextField {
    Layout.fillWidth: true
    id: certificatePath
    onTextChanged: model.certificate.path = text
  }

  Label { text: "Certificate password" }
  TextField {
    Layout.fillWidth: true
    id: certificatePassword
    onTextChanged: model.certificate.password = text
  }

  Label { text: "TicketBAI License" }
  TextField {
    Layout.fillWidth: true
    id: license
    onTextChanged: model.software.license = text
  }

  Label { text: "Device UID" }
  TextField {
    Layout.fillWidth: true
    id: deviceUid
    onTextChanged: model.software.deviceUid = text
  }

  Label { text: "Developer CIF" }
  TextField {
    Layout.fillWidth: true
    id: developerCif
    onTextChanged: model.software.cif = text;
  }

  Label { text: "Developer Name" }
  TextField {
    Layout.fillWidth: true
    id: developerName
    onTextChanged: model.software.name = text;
  }

  Label { text: "Company" }
  ComboBox {
    Layout.fillWidth: true
    id: company
    textRole: "name"
    valueRole: "id"
    model: contacts.list
    onCurrentIndexChanged: if (ready) { ticketBai.companyId = contacts.list[currentIndex].id }
  }

  Label { text: "Developer" }
  ComboBox {
    Layout.fillWidth: true
    id: developer
    textRole: "name"
    valueRole: "id"
    model: contacts.list
    onCurrentIndexChanged: if (ready) { ticketBai.developerId = contacts.list[currentIndex].id; }
  }

  Button {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Confirm"
    onClicked: application.back()
  }
}

