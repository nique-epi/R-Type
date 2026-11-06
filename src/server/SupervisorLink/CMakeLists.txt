add_library(rtype_server_supervisor_link_module STATIC
  GameProcessWatch.cpp
  ServerSupervisorLinkModule.cpp
)

target_include_directories(rtype_server_supervisor_link_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervisor_link_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_transport
    rtype_server_games_module
    rtype_server_launch_options
    rtype_server_metrics_report
    rtype_server_network_module
    rtype_server_platform
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_game_status
    rtype_network_master_messages
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_supervisor_link_module)
