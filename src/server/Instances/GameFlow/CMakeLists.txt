add_library(rtype_server_game_flow STATIC GameFlow.cpp)

target_include_directories(rtype_server_game_flow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_flow
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_systems
    rtype_engine_time
    rtype_game
    rtype_network_transport
    rtype_server_instances
    rtype_server_bydo_gunner
    rtype_server_bydo_spawner
    rtype_server_life_keeper
    rtype_server_missile_launcher
    rtype_server_player_roster
  PRIVATE
    rtype_server_ship_input
    rtype_identifiers
    rtype_network_game_started
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_game_flow)
