add_subdirectory(Context)

add_library(rtype_server_instances STATIC DelayedInputs.cpp GameInstance.cpp GameModules.cpp)

target_include_directories(rtype_server_instances PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances
  PUBLIC
    rtype_engine
    rtype_engine_event_bus
    rtype_network_player_input
    rtype_network_transport
    rtype_server_constants
    rtype_server_instances_context
    rtype_server_sessions
    rtype_server_sessions_roster
  PRIVATE
    rtype_game_events
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_instances)
