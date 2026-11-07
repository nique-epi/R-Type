rtype_add_test(NAME client_content_pack_tests SOURCES ContentPackTestFiles.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/FakePackServer.cpp LIBS rtype_client_content_pack rtype_client_exceptions rtype_network_constants httplib::httplib rtype_test_support TIMEOUT ${RTYPE_THREAD_TEST_TIMEOUT_SECONDS})
target_include_directories(client_content_pack_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles)

add_subdirectory(Download)
add_subdirectory(Cache)
add_subdirectory(Reading)
