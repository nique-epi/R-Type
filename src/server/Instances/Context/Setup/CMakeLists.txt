add_library(rtype_server_instances_context_setup INTERFACE)

target_include_directories(rtype_server_instances_context_setup INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances_context_setup
  INTERFACE
    rtype_engine
    rtype_game
    rtype_game_entity_types
    rtype_game_waves
    rtype_network_chat_line
    rtype_network_connection_request
    rtype_network_player_input
    rtype_network_ship_look_choice
    rtype_network_transport
    rtype_server_constants
    rtype_server_sessions_roster
)
