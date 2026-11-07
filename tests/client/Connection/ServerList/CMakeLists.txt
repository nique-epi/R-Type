target_sources(client_connection_tests PRIVATE
  GameCatalogFixture.cpp
  GameCatalogTest.cpp
  MasterUrlTest.cpp
  ServerCatalogFixture.cpp
  ServerCatalogTest.cpp
  ServerPingProbeFixture.cpp
  ServerPingProbeTest.cpp
  ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingMasterReads.cpp
  ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingServerPingProbe.cpp
)
target_include_directories(client_connection_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

rtype_add_test(NAME client_master_link_tests SOURCES MasterLinkTest.cpp MasterLinkFixture.cpp ${RTYPE_TEST_SOURCE_DIR}/master/RunningMaster.cpp ${RTYPE_TEST_SOURCE_DIR}/master/MasterDatabaseFile.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/HttpPortProbe.cpp ${RTYPE_TEST_SOURCE_DIR}/logging/LogLaunchOptionsFixtures.cpp LIBS rtype_client_server_list rtype_engine_application rtype_engine_command_configuration rtype_master_modules rtype_master_storage_module rtype_master_http_module rtype_master_constants rtype_network_protocol rtype_test_support httplib::httplib TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(client_master_link_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/master ${RTYPE_TEST_SOURCE_DIR}/doubles ${RTYPE_TEST_SOURCE_DIR}/logging)
