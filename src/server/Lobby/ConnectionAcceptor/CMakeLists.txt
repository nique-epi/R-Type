add_library(rtype_server_connection_acceptor STATIC ConnectionAcceptor.cpp)

target_include_directories(rtype_server_connection_acceptor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_connection_acceptor
  PUBLIC
    rtype_engine_time
    rtype_logging
    rtype_network_connection_refused
    rtype_network_connection_request
    rtype_network_cookie_issuer
    rtype_network_constants
    rtype_network_transport
    rtype_server_lobby_constants
    rtype_server_player_roster
  PRIVATE
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_connection_acceptor)
