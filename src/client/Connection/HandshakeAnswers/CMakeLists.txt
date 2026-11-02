add_library(rtype_client_handshake_answers STATIC HandshakeAnswers.cpp)

target_include_directories(rtype_client_handshake_answers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_handshake_answers
  PUBLIC
    rtype_client_connection
    rtype_client_connection_attempt
    rtype_client_session_inbound
    rtype_client_session_observers
    rtype_client_session_outbox
    rtype_logging
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_message_dispatcher
  PRIVATE
    rtype_network_heartbeat
)

rtype_enable_warnings(rtype_client_handshake_answers)
