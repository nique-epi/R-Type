rtype_add_test(NAME server_ban_book_tests SOURCES BanBookTest.cpp BanBookFixture.cpp ${RTYPE_TEST_SOURCE_DIR}/server/BanFixtures.cpp LIBS rtype_server_ban_book_module rtype_test_support)
target_include_directories(server_ban_book_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/server)
