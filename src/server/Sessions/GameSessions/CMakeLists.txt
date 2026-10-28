add_library(rtype_server_game_sessions STATIC GameSessions.cpp)

target_include_directories(rtype_server_game_sessions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_sessions
  PUBLIC
    rtype_engine_event_bus
    rtype_network_message_dispatcher
    rtype_network_transport
    rtype_server_datagram_reader
    rtype_server_departures
    rtype_server_input_keeper
    rtype_server_player_roster
    rtype_server_ready_keeper
    rtype_server_sessions
  PRIVATE
    rtype_network_disconnection
    rtype_network_heartbeat
    rtype_server_heartbeats
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_game_sessions)
