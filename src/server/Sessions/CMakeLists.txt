add_subdirectory(Roster)

add_library(rtype_server_sessions STATIC
  DatagramReader.cpp
  Departures.cpp
  GameSessions.cpp
  Heartbeats.cpp
  InputKeeper.cpp
  ReadyKeeper.cpp
)

target_include_directories(rtype_server_sessions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_sessions
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_time
    rtype_game_events
    rtype_logging
    rtype_logging_throttled_log
    rtype_network_game_loaded
    rtype_network_message_dispatcher
    rtype_network_player_input
    rtype_network_ready_state
    rtype_network_transport
    rtype_server_roster
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_exceptions
    rtype_network_heartbeat
)

rtype_enable_warnings(rtype_server_sessions)
