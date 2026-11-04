add_library(rtype_tools_load_test_bots_play STATIC
  BotInputSender.cpp
  BotPilot.cpp
  BotTally.cpp
)

target_include_directories(rtype_tools_load_test_bots_play PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_play PUBLIC rtype_client_connection_module)

rtype_enable_warnings(rtype_tools_load_test_bots_play)

rtype_add_test(NAME load_test_bots_play_tests SOURCES BotPilotTest.cpp BotPilotFixtures.cpp BotTallyTest.cpp BotTallyFixtures.cpp BotInputSenderTest.cpp BotInputSenderFixture.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingWaitingRoom.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RefusingPlayerInputLink.cpp LIBS rtype_tools_load_test_bots_play)

target_include_directories(load_test_bots_play_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles)
