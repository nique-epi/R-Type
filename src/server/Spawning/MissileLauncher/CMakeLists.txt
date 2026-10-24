add_library(rtype_server_missile_launcher STATIC MissileLauncher.cpp)

target_include_directories(rtype_server_missile_launcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_missile_launcher
  PUBLIC
    rtype_game
    rtype_logging
    rtype_server_entity_spawn_sender
    rtype_server_player_roster
  PRIVATE
    rtype_game_spawning
    rtype_server_missile_launcher_constants
)

rtype_enable_warnings(rtype_server_missile_launcher)
