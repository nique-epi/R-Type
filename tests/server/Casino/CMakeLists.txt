if(NOT RTYPE_MODULE_CASINO)
  return()
endif()

rtype_add_test(NAME server_casino_tests SOURCES RouletteTableTest.cpp RouletteTableFixture.cpp RouletteRoundAnnouncerTest.cpp RouletteRoundAnnouncerFixture.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/ScriptedRouletteDraw.cpp LIBS rtype_server_casino_module server_test_datagrams)
target_include_directories(server_casino_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/server ${RTYPE_TEST_SOURCE_DIR}/doubles)
