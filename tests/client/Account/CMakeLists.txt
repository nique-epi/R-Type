rtype_add_test(NAME client_account_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeAccountSession.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingAccountLink.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingCasinoLink.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/FakeWallet.cpp LIBS rtype_client_account_casino rtype_network_protocol rtype_test_support)
target_include_directories(client_account_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles)

add_subdirectory(Session)
add_subdirectory(Wallet)
add_subdirectory(Casino)
