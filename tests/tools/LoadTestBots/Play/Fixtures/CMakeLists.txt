target_sources(load_test_bots_play_tests PRIVATE
  BotInputSenderFixture.cpp
  BotPilotFixtures.cpp
  BotTallyFixtures.cpp
)

target_include_directories(load_test_bots_play_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
