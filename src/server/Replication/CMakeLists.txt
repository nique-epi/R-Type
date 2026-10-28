add_library(rtype_server_replication_constants INTERFACE)

target_include_directories(rtype_server_replication_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_constants
  INTERFACE
    rtype_engine_time
    rtype_network_protocol
)

add_library(rtype_server_entity_spawn_sender STATIC EntitySpawnSender.cpp)

target_include_directories(rtype_server_entity_spawn_sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_entity_spawn_sender
  PUBLIC
    rtype_network_datagram_codec
    rtype_network_entity_spawned
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_entity_spawn_sender)

add_subdirectory(WireEntityTypeMapping)
add_subdirectory(WorldStateSender)
add_subdirectory(GameEventEncoder)
add_subdirectory(GameEventBroadcaster)
add_subdirectory(ShipSpawnAnnouncer)
add_subdirectory(LobbyStateAnnouncer)
add_subdirectory(GameEndAnnouncer)
