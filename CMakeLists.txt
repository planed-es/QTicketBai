cmake_minimum_required(VERSION 3.14)

project(ticketbai-qt
  LANGUAGES CXX
  VERSION 1.0.0
  DESCRIPTION "TicketBai and LROE protocols implementation using Qt")

option(BUILD_TESTS       "builds tests" ON)
option(WITH_GUI          "adds QRCode generation" ON)
option(WITH_SIGNING      "enable TicketBAI document signing and sending" ON)
option(WITH_LROE_SENDING "enable LROE implementation" ON)

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
#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(QT NAMES Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

message("Building ${PROJECT_NAME} using Qt${QT_VERSION_MAJOR}")
set(TARGET_NAME ticketbai-qt${QT_VERSION_MAJOR})

if (${WITH_SIGNING})
  message("- Building ${TARGET_NAME} with TicketBAI document signing support")
  if (NOT DEFINED XMLSECQT_LIBRARIES)
    pkg_check_modules(XMLSECQT REQUIRED xmlsec-qt${QT_VERSION_MAJOR}>=1.0.0)
  endif()
endif()

if (${WITH_LROE_SENDING})
  message("- Building ${TARGET_NAME} with LROE support")
  find_package(ZLIB REQUIRED)
  if (NOT DEFINED QTCURL_LIBRARIES)
    pkg_check_modules(QTCURL REQUIRED Qt${QT_VERSION_MAJOR}Curl>=1.0.0)
  endif()
endif()

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${XMLSECQT_INCLUDE_DIRS})
include_directories(${QTCURL_INCLUDE_DIRS})

file(GLOB headers ticketbai-qt/*.h)
file(GLOB sources ticketbai-qt/*.cpp)

if ("${Qt6Gui_FOUND}" OR "${Qt5Gui_FOUND}")
  message("- Building ${TARGET_NAME} with QR-Code generator")
  include_directories(Qt-QrCodeGenerator/src)
  file(GLOB_RECURSE qrcode_sources Qt-QrCodeGenerator/src/*.cpp)
endif()

add_library(${TARGET_NAME} SHARED
  ${headers}
  ${sources} ${qrcode_sources}
)

target_compile_definitions(${TARGET_NAME} PRIVATE TICKETBAIQT_LIBRARY)
if (${WITH_SIGNING})
  target_compile_definitions(${TARGET_NAME} PRIVATE TICKETBAIQT_WITH_SIGNING)
endif()
if (${WITH_LROE_SENDING})
  target_compile_definitions(${TARGET_NAME} PRIVATE TICKETBAIQT_WITH_LROE_SENDING)
endif()

set(QT_COMPONENTS_LINK
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml)

if (${WITH_GUI})
  set(QT_COMPONENTS_LINK ${QT_COMPONENTS_LINK}
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Svg)
endif()

target_link_libraries(${TARGET_NAME} PRIVATE
  ${QT_COMPONENTS_LINK}
  ${ZLIB_LIBRARIES}
  ${XMLSECQT_LIBRARIES}
  ${QTCURL_LIBRARIES}
)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

string(REPLACE ";" " " XMLSECQT_CFLAGS_ALT "${XMLSECQT_CFLAGS}")
string(REPLACE ";" " " XMLSECQT_LDFLAGS_ALT "${XMLSECQT_LDFLAGS}")
configure_file(ticketbai-qt.pc.in ${TARGET_NAME}.pc @ONLY)

install(FILES ${headers} DESTINATION include/ticketbai-qt)
install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc DESTINATION lib/pkgconfig)
