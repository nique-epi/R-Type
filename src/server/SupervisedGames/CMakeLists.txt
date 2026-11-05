add_library(rtype_server_supervised_games_constants INTERFACE)

target_include_directories(rtype_server_supervised_games_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_constants INTERFACE rtype_engine_time)

add_subdirectory(Registry)
add_subdirectory(Launch)
add_subdirectory(Stopping)
add_subdirectory(Status)

add_library(rtype_server_supervised_games_module STATIC ServerSupervisedGamesModule.cpp)

target_include_directories(rtype_server_supervised_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_transport
    rtype_server_network_module
    rtype_server_processes
    rtype_server_supervised_games_launch
    rtype_server_supervised_games_registry
    rtype_server_supervised_games_status
    rtype_server_supervised_games_stopping
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_game_status_acknowledged
    rtype_server_constants
    rtype_server_supervised_games_constants
)

rtype_enable_warnings(rtype_server_supervised_games_module)
