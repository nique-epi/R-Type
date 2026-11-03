add_library(rtype_server_replication_wire STATIC
  EntitySpawnSender.cpp
  GameEventEncoder.cpp
  WireEntityTypeMapping.cpp
)

target_include_directories(rtype_server_replication_wire PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_wire
  PUBLIC
    rtype_engine_time
    rtype_game_components
    rtype_game_events
    rtype_identifiers
    rtype_network_datagram_codec
    rtype_network_messages
    rtype_network_protocol
    rtype_network_transport
    rtype_server_sessions_roster
)

rtype_enable_warnings(rtype_server_replication_wire)
