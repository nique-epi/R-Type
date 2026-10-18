add_library(rtype_server_ship_spawn_announcer STATIC ShipSpawnAnnouncer.cpp)

target_include_directories(rtype_server_ship_spawn_announcer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_ship_spawn_announcer
  PUBLIC
    rtype_game
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_game_spawning
    rtype_identifiers
    rtype_network_datagram_codec
    rtype_network_entity_spawned
)

rtype_enable_warnings(rtype_server_ship_spawn_announcer)
