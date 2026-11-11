add_library(rtype_game_scripting_level STATIC
  LuauLevelRun.cpp
  LuauLevels.cpp
)

target_include_directories(rtype_game_scripting_level PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_scripting_level
  PUBLIC
    rtype_engine_component_registry
    rtype_engine_scripting
    rtype_game_level
  PRIVATE
    rtype_engine_exceptions
    rtype_game_spawning
    rtype_identifiers
)

rtype_enable_warnings(rtype_game_scripting_level)
