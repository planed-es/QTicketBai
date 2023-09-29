import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
  id: root
  property QtObject model
  property bool ready: false
  property int columnCount: 6

  signal requestDeletion()

  Component.onCompleted: {
    refresh();
    ready = true;
  }

  function refresh()
  {
    labelInput.text = model.label;
    unitPriceInput.text = model.unitPrice;
    quantityInput.text = model.quantity;
    taxRateInput.text = model.taxRate;
    productInput.currentIndex = model.product ? productInput.indexOfValue(model.product.name) : -1;
  }

  TextField {
    id: labelInput
    onTextChanged: model.label = text
    Layout.preferredWidth: application.width / columnCount
  }

  ComboBox {
    id: productInput
    Layout.preferredWidth: application.width / columnCount
    valueRole: "name"
    textRole: "name"
    model: products.list
    onCurrentIndexChanged: {
      if (ready) { root.model.product = products.list[currentIndex] }
      root.refresh();
      console.log("Selected product:", products.list[currentIndex]);
      console.log("product =", root.model.product, ready);
    }
  }

  TextField {
    id: unitPriceInput
    validator: DoubleValidator {}
    onTextChanged: model.unitPrice = parseFloat(text)
    Layout.preferredWidth: application.width / columnCount
  }

  TextField {
    id: quantityInput
    validator: DoubleValidator {}
    onTextChanged: model.quantity = parseFloat(text)
    Layout.preferredWidth: application.width / columnCount
  }

  TextField {
    id: taxRateInput
    validator: DoubleValidator {}
    onTextChanged: model.taxRate = parseFloat(text)
    Layout.preferredWidth: application.width / columnCount
  }

  Button {
    id: deleteButton
    text: "x"
    onClicked: requestDeletion()
  }
}
