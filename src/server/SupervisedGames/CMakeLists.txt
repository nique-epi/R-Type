add_library(rtype_server_supervised_games_constants INTERFACE)

target_include_directories(rtype_server_supervised_games_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_constants INTERFACE rtype_engine_time)

add_subdirectory(Registry)
add_subdirectory(Launch)
add_subdirectory(Stopping)
add_subdirectory(Status)

add_library(rtype_server_supervised_games_module STATIC
  MeasureMilliseconds.cpp
  ServerSupervisedGamesModule.cpp
  SupervisedGamesReport.cpp
  SupervisedGamesWatch.cpp
)

target_include_directories(rtype_server_supervised_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_module
  PUBLIC
    rtype_concurrency_latest_value
    rtype_engine_commands
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_connection_refused
    rtype_network_connection_request
    rtype_network_game_status
    rtype_network_master_messages
    rtype_network_transport
    rtype_server_games_module
    rtype_server_information_module
    rtype_server_network_module
    rtype_server_processes
    rtype_server_supervised_games_launch
    rtype_server_supervised_games_registry
    rtype_server_supervised_games_status
    rtype_server_supervised_games_stopping
  PRIVATE
    rtype_engine_launch_arguments
    rtype_engine_paths
    rtype_network_constants
    rtype_network_context
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_exceptions
    rtype_network_game_status_acknowledged
    rtype_network_sequence_order
    rtype_network_udp_socket
    rtype_server_constants
    rtype_server_exceptions
    rtype_server_storage_module
    rtype_server_supervised_games_constants
)

rtype_enable_warnings(rtype_server_supervised_games_module)
