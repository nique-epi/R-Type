target_sources(game_scripting_tests PRIVATE
  LuauComponentFieldsTest.cpp
  LuauEntityHandleTest.cpp
  LuauScriptedBodies.cpp
)

target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
