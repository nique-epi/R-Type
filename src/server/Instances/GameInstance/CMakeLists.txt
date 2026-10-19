add_library(rtype_server_game_instance STATIC GameInstance.cpp)

target_include_directories(rtype_server_game_instance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_instance
  PUBLIC
    rtype_engine_time
    rtype_game
    rtype_network_datagram_codec
    rtype_network_message_dispatcher
    rtype_network_ready_state
    rtype_network_transport
    rtype_server_instances
    rtype_server_bydo_spawner
    rtype_server_player_roster
    rtype_server_ship_spawn_announcer
  PRIVATE
    rtype_identifiers
    rtype_network_exceptions
    rtype_network_sequence_order
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_game_instance)
