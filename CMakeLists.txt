cmake_minimum_required(VERSION 3.14)

project(ticketbai-qt LANGUAGES CXX)

option(BUILD_TESTS "builds tests" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(ZLIB REQUIRED)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Network Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Xml)
pkg_check_modules(XMLSECQT REQUIRED xmlsec-qt>=1.0.0)
pkg_check_modules(QTCURL REQUIRED QtCurl>=1.0.0)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${XMLSECQT_INCLUDE_DIRS})
include_directories(${QTCURL_INCLUDE_DIRS})

file(GLOB headers ticketbai-qt/*.h)
file(GLOB sources ticketbai-qt/*.cpp)

add_library(ticketbai-qt SHARED
  ${headers}
  ${sources}
)

target_link_libraries(ticketbai-qt PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  QtCurl
  z
  ${XMLSECQT_LIBRARIES}
  ${QTCURL_LIBRARIES}
)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

install(FILES ${headers} DESTINATION include/ticketbai-qt)
install(TARGETS ticketbai-qt LIBRARY DESTINATION lib)
