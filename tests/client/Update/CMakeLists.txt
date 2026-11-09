if(NOT RTYPE_MODULE_UPDATE)
  return()
endif()

rtype_add_test(NAME client_update_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeUpdateRun.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingInterfaceCanvas.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingNavigator.cpp LIBS rtype_client_update_release rtype_client_update_install rtype_client_update_screens rtype_network_byte_text OpenSSL::Crypto GTest::gmock)
target_include_directories(client_update_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/doubles)

add_subdirectory(Release)
add_subdirectory(Install)
add_subdirectory(Screens)
