add_library(rtype_game_damage_rules STATIC DamageRules.cpp)

target_include_directories(rtype_game_damage_rules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_damage_rules
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_component_registry
    rtype_game_components
    rtype_game_events
)

rtype_enable_warnings(rtype_game_damage_rules)
