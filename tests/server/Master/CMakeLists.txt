if(NOT RTYPE_MODULE_MASTER_LINK)
  return()
endif()

rtype_add_test(NAME server_master_tests SOURCES MasterMessagesTest.cpp HeartbeatJobTest.cpp HeartbeatJobFixture.cpp ResultsCourierTest.cpp MasterProbeDeskTest.cpp ${RTYPE_TEST_SOURCE_DIR}/server/Storage/Database/ResultsFileFixtures.cpp MasterProbeDeskFixtures.cpp ${RTYPE_TEST_SOURCE_DIR}/master/RunningMaster.cpp ${RTYPE_TEST_SOURCE_DIR}/master/MasterDatabaseFile.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/HttpPortProbe.cpp ${RTYPE_TEST_SOURCE_DIR}/logging/LogLaunchOptionsFixtures.cpp LIBS rtype_server_master_module rtype_master_modules rtype_master_storage_module rtype_master_http_module rtype_master_constants rtype_test_support httplib::httplib TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(server_master_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/server/Storage/Database ${RTYPE_TEST_SOURCE_DIR}/master ${RTYPE_TEST_SOURCE_DIR}/doubles ${RTYPE_TEST_SOURCE_DIR}/logging)
