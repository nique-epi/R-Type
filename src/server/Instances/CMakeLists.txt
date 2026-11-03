add_library(rtype_server_instances STATIC GameInstance.cpp GameModules.cpp)

target_include_directories(rtype_server_instances PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances
  PUBLIC
    rtype_engine
    rtype_engine_event_bus
    rtype_game
    rtype_game_bydo_kind
    rtype_game_waves
    rtype_network_transport
    rtype_server_constants
    rtype_server_delayed_inputs
    rtype_server_game_sessions
    rtype_server_player_roster
)

rtype_enable_warnings(rtype_server_instances)
