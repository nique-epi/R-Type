add_library(rtype_client_handshake STATIC
  ConnectionAttempt.cpp
  GameRequest.cpp
  HandshakeAnswers.cpp
  SessionLossWatch.cpp
)

target_include_directories(rtype_client_handshake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_handshake
  PUBLIC
    rtype_client_connection_entity_types
    rtype_client_content_pack
    rtype_client_session
    rtype_engine_time
    rtype_logging
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_connection_redirect
    rtype_network_connection_refused
    rtype_network_connection_request
    rtype_network_message_dispatcher
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_engine_exceptions
    rtype_network_lobby_state
)

rtype_enable_warnings(rtype_client_handshake)
