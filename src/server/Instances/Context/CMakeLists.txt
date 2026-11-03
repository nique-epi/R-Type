add_library(rtype_server_instances_context INTERFACE)

target_include_directories(rtype_server_instances_context INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances_context
  INTERFACE
    rtype_engine
    rtype_engine_event_bus
    rtype_game
    rtype_game_bydo_kind
    rtype_game_waves
    rtype_network_transport
    rtype_server_constants
    rtype_server_sessions_roster
)
