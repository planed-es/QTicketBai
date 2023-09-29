import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Application 1.0

GridLayout {
  id: root
  property QtObject model
  property bool ready: false

  columns: 2
  columnSpacing: 5
  rowSpacing: 5

  Component.onCompleted: {
    nameInput.text = model.name;
    addressInput.text = model.address;
    cityInput.text = model.city;
    idTypeInput.currentIndex = idTypeInput.indexOfValue(model.idType);
    idInput.text = model.id;
    phoneInput.text = model.phone;
    emailInput.text = model.email;
    postalCodeInput.text = model.postalCode;
    countryCodeInput.text = model.countryCode;
    ready = true;
  }

  Label {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Contact"
    font.pixelSize: 21
  }

  Label { text: "Name" }
  TextField {
    Layout.fillWidth: true
    id: nameInput
    onTextChanged: model.name = text
  }

  Label { text: "Address" }
  TextField {
    Layout.fillWidth: true
    id: addressInput
    onTextChanged: model.address = text
  }

  Label { text: "City" }
  TextField {
    Layout.fillWidth: true
    id: cityInput
    onTextChanged: model.city = text
  }

  Label { text: "Postal code" }
  TextField {
    Layout.fillWidth: true
    id: postalCodeInput
    onTextChanged: model.postalCode = text
  }

  Label { text: "Country code" }
  TextField {
    Layout.fillWidth: true
    id: countryCodeInput
    onTextChanged: model.countryCode = text
  }

  Label { text: "ID Type" }
  ComboBox {
    Layout.fillWidth: true
    id: idTypeInput
    textRole: "text"
    valueRole: "value"
    onCurrentValueChanged: if (ready) { root.model.idType = model[currentIndex].value }
    model: [
      { text: "NIF", value: ContactDefines.NifIvaId },
      { text: "Passport", value: ContactDefines.PassportId },
      { text: "Official identification document from country of residence", value: ContactDefines.OfficialIdentificationDocumentFromCountryOfResidenceId },
      { text: "Residence certificate", value: ContactDefines.ResidenceCertificateId },
      { text: "Other supporting document", value: ContactDefines.OtherSupportingDocument }
    ]
  }

  Label { text: "ID" }
  TextField {
    Layout.fillWidth: true
    id: idInput
    onTextChanged: model.id = text
  }

  Label { text: "Phone" }
  TextField {
    Layout.fillWidth: true
    id: phoneInput
    onTextChanged: model.phone = text
  }

  Label { text: "Email" }
  TextField {
    Layout.fillWidth: true
    id: emailInput
    onTextChanged: model.email = text
  }

  Button {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Confirm"
    onClicked: application.back()
  }
}

