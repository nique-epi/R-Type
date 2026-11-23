rtype_add_test(NAME server_content_tests SOURCES ContentPackDeskTest.cpp ContentPackDeskFixtures.cpp ${RTYPE_TEST_SOURCE_DIR}/network/Content/Pack/ContentPackFixtures.cpp LIBS rtype_server_content_module rtype_network_content)
target_include_directories(server_content_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/network/Content/Pack)
