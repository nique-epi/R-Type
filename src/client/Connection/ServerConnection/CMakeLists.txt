add_library(rtype_client_server_connection STATIC ServerConnection.cpp)

target_include_directories(rtype_client_server_connection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_server_connection
  PUBLIC
    rtype_client_connection
    rtype_client_game_event_history
    rtype_engine_time
    rtype_logging
    rtype_network_message_dispatcher
    rtype_network_transport
  PRIVATE
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_connection_request
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_exceptions
    rtype_network_game_loaded
    rtype_network_game_started
    rtype_network_heartbeat
    rtype_network_player_input
    rtype_network_ready_state
    rtype_network_sequence_order
    rtype_network_world_state_codec
)

rtype_enable_warnings(rtype_client_server_connection)
