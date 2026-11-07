add_subdirectory(Behaviours)
add_subdirectory(Components)
add_subdirectory(EnemyActions)
add_subdirectory(EntityTypes)
add_subdirectory(Events)
add_subdirectory(Exceptions)
add_subdirectory(ShipControl)
add_subdirectory(Spawning)
add_subdirectory(Systems)
add_subdirectory(Waves)

add_library(rtype_game STATIC World.cpp)

target_include_directories(rtype_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game
  PUBLIC
    rtype_engine
    rtype_game_components
    rtype_game_entity_types
    rtype_game_events
    rtype_game_behaviours
    rtype_game_movement_system
    rtype_game_pilot_system
    rtype_game_playfield_exit_system
    rtype_game_collision_system
    rtype_game_collision_rules
    rtype_game_lag_compensated_hit_system
    rtype_game_hurt_zone_system
    rtype_game_damage_rules
    rtype_game_boss_phase_system
)

rtype_enable_warnings(rtype_game)
