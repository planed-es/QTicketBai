find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Core Network Xml)
find_package(ZLIB REQUIRED)

enable_testing(true)
include_directories(../)

set(test_libraries 
  ${TARGET_NAME}
  Qt${QT_VERSION_MAJOR}::Test
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  ${QTCURL_LIBRARIES}
  z
  ${XMLSECQT_LIBRARIES}
  ${CRAILS_ENCRYPT_LIBRARIES}
)

add_executable(lroeclient
  tst_lroeclient.cpp
  helper_tbai_test.cpp
  helper_xmlsec_test.cpp
)
add_test(NAME lroeclient COMMAND lroeclient)
target_link_libraries(lroeclient PRIVATE ${test_libraries})

add_executable(tbaiinvoice
  tst_tbaiinvoice.cpp
  helper_tbai_test.cpp
)
add_test(NAME tbaiinvoice COMMAND tbaiinvoice)
target_link_libraries(tbaiinvoice PRIVATE ${test_libraries})

add_executable(tbaicertificate
  tst_tbaicertificate.cpp
)
add_test(NAME tbaicertificate COMMAND tbaicertificate)
target_link_libraries(tbaicertificate PRIVATE ${test_libraries})

add_executable(tbaisignprocess
  tst_tbaisignprocess.cpp
  helper_tbai_test.cpp
  helper_xmlsec_test.cpp
)
add_test(NAME tbaisignprocess COMMAND tbaisignprocess)
target_link_libraries(tbaisignprocess PRIVATE ${test_libraries})
