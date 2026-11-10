if(NOT RTYPE_MODULE_UPDATE)
  return()
endif()

rtype_add_test(NAME client_update_tests
  SOURCES
    ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeReleaseFiles.cpp
    ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeReleaseUpdater.cpp
    ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeUpdateRun.cpp
    ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingInterfaceCanvas.cpp
    ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingNavigator.cpp
  LIBS
    rtype_client_exceptions
    rtype_client_update_feed
    rtype_client_update_run
    rtype_client_update_screens
    rtype_test_support
    GTest::gmock
)
target_include_directories(client_update_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/doubles)

add_subdirectory(Feed)
add_subdirectory(Run)
add_subdirectory(Screens)
