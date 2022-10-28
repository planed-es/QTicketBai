find_package(Qt6 REQUIRED COMPONENTS Test Core Network Xml)
find_package(ZLIB REQUIRED)

enable_testing(true)
include_directories(..)

add_executable(lroesubmitprocess tst_lroesubmitprocess.cpp
  ../ticketbai-qt/lroesubmitprocess.cpp
  ../ticketbai-qt/lroedocument.cpp
  ../ticketbai-qt/tbaicertificate.cpp
  ../ticketbai-qt/tbaidocument.cpp
  ../ticketbai-qt/invoiceuploaddocument.cpp
  ../ticketbai-qt/tbaisignprocess.cpp
  ../ticketbai-qt/qcompressor.cpp
)
add_test(NAME lroesubmitprocess COMMAND lroesubmitprocess)
target_link_libraries(lroesubmitprocess PRIVATE
  Qt${QT_VERSION_MAJOR}::Test
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  QtCurl
  z
  curl)
