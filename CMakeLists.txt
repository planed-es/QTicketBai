cmake_minimum_required(VERSION 3.14)

project(ticketbai-qt
  LANGUAGES CXX
  VERSION 1.0.0
  DESCRIPTION "TicketBai and LROE protocols implementation using Qt")

option(BUILD_TESTS "builds tests" ON)
option(WITH_GUI    "adds QRCode generation" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_COMPONENTS Core Network Xml)
if (${WITH_GUI})
  set(QT_COMPONENTS ${QT_COMPONENTS} Gui Svg)
endif()

find_package(PkgConfig)
find_package(ZLIB REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})
pkg_check_modules(XMLSECQT REQUIRED xmlsec-qt>=1.0.0)
pkg_check_modules(QTCURL REQUIRED QtCurl>=1.0.0)

message("Using Qt${QT_VERSION_MAJOR}")

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${XMLSECQT_INCLUDE_DIRS})
include_directories(${QTCURL_INCLUDE_DIRS})

file(GLOB headers ticketbai-qt/*.h)
file(GLOB sources ticketbai-qt/*.cpp)

if ("${Qt6Gui_FOUND}" OR "${Qt5Gui_FOUND}")
  include_directories(Qt-QrCodeGenerator/src)
  file(GLOB_RECURSE qrcode_sources Qt-QrCodeGenerator/src/*.cpp)
endif()

add_library(ticketbai-qt SHARED
  ${headers}
  ${sources} ${qrcode_sources}
)

target_compile_definitions(ticketbai-qt PRIVATE TICKETBAIQT_LIBRARY)

set(QT_COMPONENTS_LINK
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml)

if (${WITH_GUI})
  set(QT_COMPONENTS_LINK ${QT_COMPONENTS_LINK}
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Svg)

endif()

target_link_libraries(ticketbai-qt PRIVATE
  ${QT_COMPONENTS_LINK}
  z
  ${XMLSECQT_LIBRARIES}
  ${QTCURL_LIBRARIES}
)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

configure_file(ticketbai-qt.pc.in ticketbai-qt.pc @ONLY)

install(FILES ${headers} DESTINATION include/ticketbai-qt)
install(TARGETS ticketbai-qt LIBRARY DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/ticketbai-qt.pc DESTINATION lib/pkgconfig)
