cmake_minimum_required(VERSION 3.14)

project(qt-ticketbai-example LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig)
find_package(QT NAMES Qt6 COMPONENTS Core Gui Qml QuickControls2 Network Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Network Xml)

pkg_check_modules(XMLSECQT REQUIRED xmlsec-qt>=1.0.0)

include_directories(${XMLSECQT_INCLUDE_DIRS})

set(PROJECT_SOURCES
  invoice.h invoice.cpp
  invoiceline.h invoiceline.cpp
  invoicecontroller.h invoicecontroller.cpp
  product.h product.cpp
  productcontroller.h productcontroller.cpp
  contact.h contact.cpp
  contactcontroller.h contactcontroller.cpp
  settings.h settings.cpp
  main.cpp
  qml.qrc)

add_executable(qt-ticketbai-example ${PROJECT_SOURCES})

target_link_libraries(qt-ticketbai-example
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Network
  ${XMLSECQT_LIBRARIES}
  QtCurl
  curl
  ticketbai-qt)
