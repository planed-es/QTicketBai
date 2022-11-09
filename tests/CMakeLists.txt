find_package(Qt6 REQUIRED COMPONENTS Test Core Network Xml)
find_package(ZLIB REQUIRED)

enable_testing(true)
include_directories(..)

set(test_libraries 
  Qt${QT_VERSION_MAJOR}::Test
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  QtCurl
  z
  curl
  ${XMLSECQT_LIBRARIES}
  ${CRAILS_ENCRYPT_LIBRARIES}
)

add_executable(lroesubmitprocess
  tst_lroesubmitprocess.cpp
  helper_tbai_test.cpp
  helper_xmlsec_test.cpp
  ../ticketbai-qt/qticketbai.cpp
  ../ticketbai-qt/lroesubmitprocess.cpp
  ../ticketbai-qt/lroeclient.cpp
  ../ticketbai-qt/lroedocument.cpp
  ../ticketbai-qt/tbaicertificate.cpp
  ../ticketbai-qt/abstracttbaidocument.cpp
  ../ticketbai-qt/tbaidocument.cpp
  ../ticketbai-qt/uploaddocument.cpp
  ../ticketbai-qt/canceldocument.cpp
  ../ticketbai-qt/tbaisignprocess.cpp
  ../ticketbai-qt/qcompressor.cpp
)
add_test(NAME lroesubmitprocess COMMAND lroesubmitprocess)
target_link_libraries(lroesubmitprocess PRIVATE ${test_libraries})

add_executable(tbaiinvoice
  tst_tbaiinvoice.cpp
  helper_tbai_test.cpp
  ../ticketbai-qt/qticketbai.cpp
  ../ticketbai-qt/tbaiinvoice.cpp
  ../ticketbai-qt/invoicecomponent.cpp)
add_test(NAME tbaiinvoice COMMAND tbaiinvoice)
target_link_libraries(tbaiinvoice PRIVATE ${test_libraries})

add_executable(tbaicertificate
  tst_tbaicertificate.cpp
  ../ticketbai-qt/tbaicertificate.cpp)
add_test(NAME tbaicertificate COMMAND tbaicertificate)
target_link_libraries(tbaicertificate PRIVATE ${test_libraries})

add_executable(tbaisignprocess
  tst_tbaisignprocess.cpp
  helper_tbai_test.cpp
  helper_xmlsec_test.cpp
  ../ticketbai-qt/qticketbai.cpp
  ../ticketbai-qt/tbaicertificate.cpp
  ../ticketbai-qt/abstracttbaidocument.cpp
  ../ticketbai-qt/tbaidocument.cpp
  ../ticketbai-qt/tbaisignprocess.cpp)
add_test(NAME tbaisignprocess COMMAND tbaisignprocess)
target_link_libraries(tbaisignprocess PRIVATE ${test_libraries})
