add_library(rtype_client_server_session STATIC ServerSession.cpp)

target_include_directories(rtype_client_server_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_server_session
  PUBLIC
    rtype_client_connection
    rtype_client_connection_attempt
    rtype_client_handshake_answers
    rtype_client_session_departure
    rtype_client_session_inbound
    rtype_client_session_observers
    rtype_client_session_outbox
    rtype_engine_time
    rtype_logging
    rtype_network_message_dispatcher
    rtype_network_transport
  PRIVATE
    rtype_network_player_input
)

rtype_enable_warnings(rtype_client_server_session)
