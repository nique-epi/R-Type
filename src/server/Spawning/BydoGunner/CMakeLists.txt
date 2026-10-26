add_library(rtype_server_bydo_gunner STATIC BydoGunner.cpp)

target_include_directories(rtype_server_bydo_gunner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_bydo_gunner
  PUBLIC
    rtype_game
    rtype_logging
    rtype_server_entity_spawn_sender
    rtype_server_player_roster
  PRIVATE
    rtype_game_enemy_missile_start_position
    rtype_server_bydo_gunner_constants
)

rtype_enable_warnings(rtype_server_bydo_gunner)
