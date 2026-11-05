add_library(rtype_server_replication_announcements_streams STATIC
  GameEventBroadcaster.cpp
  WorldStateSender.cpp
)

target_include_directories(rtype_server_replication_announcements_streams PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_announcements_streams
  PUBLIC
    rtype_engine
    rtype_engine_time
    rtype_game
    rtype_logging
    rtype_network_exceptions
    rtype_network_protocol
    rtype_network_transport
    rtype_network_world_state_codec
    rtype_network_world_state_delta
    rtype_server_constants
    rtype_server_exceptions
    rtype_server_replication_wire
    rtype_server_sessions_roster
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_server_replication_announcements_streams)
