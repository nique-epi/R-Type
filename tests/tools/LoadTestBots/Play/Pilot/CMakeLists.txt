add_library(rtype_tools_load_test_bots_play_pilot STATIC
  ActionChoice.cpp
  BotInputSender.cpp
  BotPilot.cpp
  RulePilot.cpp
)

target_include_directories(rtype_tools_load_test_bots_play_pilot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_play_pilot PUBLIC rtype_game_ship_control_constants rtype_tools_load_test_bots_play_sight)

rtype_enable_warnings(rtype_tools_load_test_bots_play_pilot)

rtype_add_test(NAME load_test_bots_play_pilot_tests SOURCES ActionChoiceTest.cpp BotPilotTest.cpp BotInputSenderTest.cpp RulePilotTest.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/FixedEntityTypeTable.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingWaitingRoom.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RefusingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/tools/LoadTestBots/Play/Sight/Fixtures/BotSightFixtures.cpp LIBS rtype_tools_load_test_bots_play_pilot)

target_include_directories(load_test_bots_play_pilot_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles ${CMAKE_SOURCE_DIR}/tests/tools/LoadTestBots/Play/Sight/Fixtures)

add_subdirectory(Fixtures)
