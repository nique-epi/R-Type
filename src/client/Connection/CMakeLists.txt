add_subdirectory(Transport)
add_subdirectory(Session)
add_subdirectory(EntityTypes)
add_subdirectory(Handshake)
add_subdirectory(WaitingRoom)
add_subdirectory(Leaderboard)

add_library(rtype_client_connection_module STATIC
  ClientConnectionModule.cpp
  PlayerConnection.cpp
  ServerSession.cpp
  UnconfirmedInputs.cpp
)

target_include_directories(rtype_client_connection_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_module
  PUBLIC
    rtype_client_connection_entity_types
    rtype_client_connection_leaderboard
    rtype_client_entity_appearances
    rtype_client_entity_messages
    rtype_client_handshake
    rtype_client_input_commands
    rtype_client_session
    rtype_client_transport
    rtype_client_waiting_room
    rtype_engine_install_context
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_engine_time
    rtype_logging
    rtype_network_conditions
    rtype_network_message_dispatcher
    rtype_network_player_input
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_client_constants
    rtype_network_connection_refused
    rtype_network_constants
    rtype_network_disconnection
)

rtype_enable_warnings(rtype_client_connection_module)
