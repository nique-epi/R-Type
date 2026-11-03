add_library(rtype_server_replication_module STATIC
  EntitySpawnSender.cpp
  GameEndAnnouncer.cpp
  GameEventBroadcaster.cpp
  GameEventEncoder.cpp
  GameReplication.cpp
  LobbyStateAnnouncer.cpp
  ReplicationModule.cpp
  ShipSpawnAnnouncer.cpp
  WireEntityTypeMapping.cpp
  WorldStateSender.cpp
)

target_include_directories(rtype_server_replication_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_module
  PUBLIC
    rtype_engine
    rtype_engine_install_context
    rtype_engine_time
    rtype_game
    rtype_game_components
    rtype_game_events
    rtype_identifiers
    rtype_logging
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_network_messages
    rtype_network_protocol
    rtype_network_transport
    rtype_network_world_state_codec
    rtype_server_constants
    rtype_server_exceptions
    rtype_server_instances
    rtype_server_roster
  PRIVATE
    rtype_game_spawning
)

rtype_enable_warnings(rtype_server_replication_module)
