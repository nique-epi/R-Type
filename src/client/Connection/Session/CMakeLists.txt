add_library(rtype_client_session STATIC
  ConnectionAttempt.cpp
  GameLoadedReminder.cpp
  HandshakeAnswers.cpp
  ServerSession.cpp
  SessionDeparture.cpp
  SessionInbound.cpp
  SessionLossWatch.cpp
  SessionObservers.cpp
  SessionOutbox.cpp
  WaitingRoom.cpp
)

target_include_directories(rtype_client_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session
  PUBLIC
    rtype_client_input_commands
    rtype_client_transport
    rtype_engine_time
    rtype_logging
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_connection_refused
    rtype_network_connection_request
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_game_loaded
    rtype_network_game_started
    rtype_network_heartbeat
    rtype_network_lobby_state
    rtype_network_message_dispatcher
    rtype_network_protocol
    rtype_network_ready_state
    rtype_network_transport
  PRIVATE
    rtype_network_exceptions
    rtype_network_player_input
    rtype_network_sequence_order
)

rtype_enable_warnings(rtype_client_session)
