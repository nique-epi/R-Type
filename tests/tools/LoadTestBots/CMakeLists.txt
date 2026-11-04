add_library(rtype_tools_load_test_bots STATIC
  Bot.cpp
  BotInputSender.cpp
  BotLaunchOptions.cpp
  BotPilot.cpp
  BotSquad.cpp
  BotTally.cpp
  ReadyRule.cpp
)

target_include_directories(rtype_tools_load_test_bots PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots
  PUBLIC
    rtype_client_connection_module
    rtype_client_constants
    rtype_server_constants
    rtype_server_launch_options
  PRIVATE
    rtype_network_constants
)

rtype_enable_warnings(rtype_tools_load_test_bots)

add_executable(load_test_bots main.cpp)

target_link_libraries(load_test_bots
  PRIVATE
    rtype_server_platform
    rtype_tools_load_test_bots
)

set_target_properties(load_test_bots PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

rtype_enable_warnings(load_test_bots)

rtype_add_test(NAME load_test_bots_tests SOURCES BotPilotTest.cpp BotPilotFixtures.cpp ReadyRuleTest.cpp BotLaunchOptionsTest.cpp BotTallyTest.cpp BotTallyFixtures.cpp BotInputSenderTest.cpp BotInputSenderFixture.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingWaitingRoom.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RefusingPlayerInputLink.cpp LIBS rtype_tools_load_test_bots)

target_include_directories(load_test_bots_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles)
