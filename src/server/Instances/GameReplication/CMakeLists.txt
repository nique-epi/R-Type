add_library(rtype_server_game_replication STATIC GameReplication.cpp)

target_include_directories(rtype_server_game_replication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_replication
  PUBLIC
    rtype_server_entity_spawn_sender
    rtype_server_game_end_announcer
    rtype_server_instances
    rtype_server_lobby_state_announcer
    rtype_server_player_roster
    rtype_server_ship_spawn_announcer
    rtype_server_world_state_sender
)

rtype_enable_warnings(rtype_server_game_replication)
