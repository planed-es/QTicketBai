import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "."

ModelIndex {
  controller: products
  modelName: "Product"
  onCreate: application.openProduct(products.createProduct())
  onOpen: application.openProduct(model)
}
