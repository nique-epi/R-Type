add_library(rtype_client_server_connection STATIC ServerConnection.cpp)

target_include_directories(rtype_client_server_connection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_server_connection
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_logging
    rtype_network_message_dispatcher
    rtype_network_transport
  PRIVATE
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_connection_request
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_network_player_input
)

rtype_enable_warnings(rtype_client_server_connection)
