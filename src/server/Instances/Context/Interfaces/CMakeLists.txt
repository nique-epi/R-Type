add_library(rtype_server_instances_context_interfaces INTERFACE)

target_include_directories(rtype_server_instances_context_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances_context_interfaces
  INTERFACE
    rtype_game
    rtype_server_instances_context_setup
    rtype_server_sessions_roster
)
