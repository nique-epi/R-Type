add_library(rtype_game_scripting_level STATIC
  LevelPlacementFiles.cpp
  LuauLevelRun.cpp
  LuauLevels.cpp
)

target_include_directories(rtype_game_scripting_level PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_scripting_level
  PUBLIC
    rtype_engine_component_registry
    rtype_engine_scripting
    rtype_game_entity_sheets
    rtype_game_level
    rtype_game_level_ldtk
  PRIVATE
    rtype_engine_data_files
    rtype_engine_exceptions
    rtype_engine_resources
    rtype_game_exceptions
    rtype_game_scripting_constants
    rtype_game_spawning
    rtype_identifiers
)

rtype_enable_warnings(rtype_game_scripting_level)
