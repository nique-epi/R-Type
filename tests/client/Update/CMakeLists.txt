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
    rtype_client_text_measures
    rtype_client_exceptions
    rtype_client_update_notes
    rtype_client_update_releases
    rtype_client_update_run
    rtype_client_update_screens
    rtype_test_support
    GTest::gmock
)
target_include_directories(client_update_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/doubles ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Releases)
add_subdirectory(Notes)
add_subdirectory(Run)
add_subdirectory(Screens)
