add_library(rtype_server_looks_module STATIC
  LooksModule.cpp
  LookRelay.cpp
)

target_include_directories(rtype_server_looks_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_looks_module
  PUBLIC
    rtype_engine_install_context
    rtype_network_ship_look_choice
    rtype_server_instances
  PRIVATE
    rtype_identifiers
    rtype_network_player_looks
    rtype_server_sessions_roster
    rtype_server_sessions_tracking
)

rtype_enable_warnings(rtype_server_looks_module)
