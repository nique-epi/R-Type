add_library(rtype_tools_load_test_bots_play_pilot STATIC
  BotInputSender.cpp
  BotPilot.cpp
)

target_include_directories(rtype_tools_load_test_bots_play_pilot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_play_pilot PUBLIC rtype_client_connection_module)

rtype_enable_warnings(rtype_tools_load_test_bots_play_pilot)

rtype_add_test(NAME load_test_bots_play_pilot_tests SOURCES BotPilotTest.cpp BotInputSenderTest.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingWaitingRoom.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RefusingPlayerInputLink.cpp LIBS rtype_tools_load_test_bots_play_pilot)

target_include_directories(load_test_bots_play_pilot_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles)

add_subdirectory(Fixtures)
