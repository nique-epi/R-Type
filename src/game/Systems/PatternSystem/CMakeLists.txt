add_library(rtype_game_pattern_system STATIC PatternSystem.cpp)

target_include_directories(rtype_game_pattern_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_pattern_system
  PUBLIC
    rtype_engine_systems
    rtype_engine_component_registry
    rtype_game_components
    rtype_game_spawning_constants
)

rtype_enable_warnings(rtype_game_pattern_system)
