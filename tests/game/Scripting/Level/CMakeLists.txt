target_sources(game_scripting_tests PRIVATE
  LuauLevelRunFixture.cpp
  LuauLevelRunTest.cpp
  LuauLevelsTest.cpp
  ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingLevelBosses.cpp
)

target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
