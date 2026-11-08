add_library(rtype_server_sessions_tracking STATIC
  DatagramReader.cpp
  Departures.cpp
  Heartbeats.cpp
  InputKeeper.cpp
  MessageDelivery.cpp
  PlayerExpeller.cpp
  ReadyKeeper.cpp
)

target_include_directories(rtype_server_sessions_tracking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_sessions_tracking
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_time
    rtype_game_events
    rtype_logging
    rtype_network_game_loaded
    rtype_network_heartbeat
    rtype_network_player_input
    rtype_network_protocol
    rtype_network_ready_state
    rtype_network_transport
    rtype_server_sessions_roster
  PRIVATE
    rtype_server_constants
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_network_round_trips
    rtype_network_statistics_round_trip
)

rtype_enable_warnings(rtype_server_sessions_tracking)
