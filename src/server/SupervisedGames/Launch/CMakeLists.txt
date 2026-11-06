add_library(rtype_server_supervised_games_launch STATIC
  FreeUdpPort.cpp
  GameCommand.cpp
  GameConfigurationFile.cpp
  GameSpawner.cpp
)

target_include_directories(rtype_server_supervised_games_launch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_launch
  PUBLIC
    rtype_logging
    rtype_server_network_module
    rtype_server_processes
    rtype_server_supervised_games_registry
  PRIVATE
    rtype_engine_launch_arguments
    rtype_engine_paths
    rtype_network_constants
    rtype_network_context
    rtype_network_transport
    rtype_network_udp_socket
    rtype_server_constants
    rtype_server_exceptions
    rtype_server_storage_module
    rtype_server_supervised_games_constants
)

rtype_enable_warnings(rtype_server_supervised_games_launch)
