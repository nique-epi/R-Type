rtype_add_test(NAME server_ban_tests SOURCES BanListTest.cpp ${RTYPE_TEST_SOURCE_DIR}/server/BanFixtures.cpp LIBS rtype_server_bans)
target_include_directories(server_ban_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/server)
