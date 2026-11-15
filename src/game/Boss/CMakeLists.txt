add_library(rtype_game_boss STATIC BossSystem.cpp BossHealth.cpp)

target_include_directories(rtype_game_boss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_boss
  PUBLIC
    rtype_engine_systems
    rtype_engine_component_registry
    rtype_engine_event_bus
    rtype_engine_time
    rtype_game_components
    rtype_game_enemy_actions
    rtype_game_events
)

rtype_enable_warnings(rtype_game_boss)
