add_library(rtype_game_enemy_actions STATIC EnemyActions.cpp)

target_include_directories(rtype_game_enemy_actions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_enemy_actions
  PUBLIC
    rtype_engine_entity_registry
    rtype_game_components
  PRIVATE
    rtype_engine_component_registry
    rtype_engine_event_bus
    rtype_game_events
)

rtype_enable_warnings(rtype_game_enemy_actions)
