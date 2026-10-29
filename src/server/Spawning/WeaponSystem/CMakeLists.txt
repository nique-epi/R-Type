add_library(rtype_server_weapon_system STATIC WeaponSystem.cpp)

target_include_directories(rtype_server_weapon_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_weapon_system
  PUBLIC
    rtype_game
    rtype_logging
    rtype_server_entity_spawn_sender
    rtype_server_player_roster
    rtype_server_weapon_shot
  PRIVATE
    rtype_game_enemy_missile_start_position
    rtype_game_spawning
    rtype_server_weapon_system_constants
)

rtype_enable_warnings(rtype_server_weapon_system)
