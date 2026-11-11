target_sources(game_scripting_tests PRIVATE
  LuauLevelRunFixture.cpp
  LuauLevelRunTest.cpp
  LuauLevelsTest.cpp
)

target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
