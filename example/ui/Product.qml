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
    defaultPriceInput.text = model.defaultPrice;
    defaultVatRateInput.text = model.defaultVat;
    console.log("Debug Product vatExemption", model.vatExemption);
    console.log("index ->", vatExemption.indexOfValue(model.vatExemption));
    vatExemption.currentIndex = vatExemption.indexOfValue(model.vatExemption);
    vatState.currentIndex = vatState.indexOfValue(model.vatState);
    ready = true;
  }

  Label {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Product"
    font.pixelSize: 21
  }

  Label { text: "Name" }
  TextField {
    Layout.fillWidth: true
    id: nameInput
    onTextChanged: model.name = text
  }

  Label { text: "Default Price" }
  TextField {
    Layout.fillWidth: true
    id: defaultPriceInput
    onTextChanged: model.defaultPrice = parseFloat(text)
  }

  Label { text: "Default VAT rate" }
  TextField {
    Layout.fillWidth: true
    id: defaultVatRateInput
    onTextChanged: model.defaultVat = parseFloat(text)
  }

  Label { text: "VAT exemption" }
  ComboBox {
    Layout.fillWidth: true
    id: vatExemption
    textRole: "text"
    valueRole: "value"
    onCurrentIndexChanged: if (ready) { root.model.vatExemption = model[currentIndex].value; }
    model: [
      { text: "None", value: InvoiceDefines.NoVatExemption },
      { text: "Norma Foral - Article 20", value: InvoiceDefines.VatExemptedByNormaForalArticle20 },
      { text: "Norma Foral - Article 21", value: InvoiceDefines.VatExemptedByNormaForalArticle21 },
      { text: "Norma Foral - Article 22", value: InvoiceDefines.VatExemptedByNormaForalArticle22 },
      { text: "Norma Foral - Article 23 and 24", value: InvoiceDefines.VatExemptedByNormaForalArticle23and24 },
      { text: "Norma Foral - Article 25", value: InvoiceDefines.VatExemptedByNormaForalArticle25 },
      { text: "Exempted", value: InvoiceDefines.VatExempted }
    ]
  }

  Label { text: "VAT state" }
  ComboBox {
    Layout.fillWidth: true
    id: vatState
    textRole: "text"
    valueRole: "value"
    onCurrentValueChanged: if (ready) { root.model.vatState = model[currentIndex].value; }
    model: [
      { text: "Subject to VAT", value: InvoiceDefines.SubjectToVat },
      { text: "Exempted by Norma Foral - Artile 7", value: InvoiceDefines.VatExemptedByNormaForalArticle7 },
      { text: "Exempted due to localization", value: InvoiceDefines.VatExemptedDueToLocalization }
    ]
  }

  Button {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: "Confirm"
    onClicked: application.back()
  }
}
