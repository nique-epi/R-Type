add_library(rtype_server_lobby_module STATIC ServerLobbyModule.cpp)

target_include_directories(rtype_server_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_lobby_module
  PUBLIC
    rtype_engine_application
    rtype_engine_system_clock
    rtype_server_application_interfaces
    rtype_server_connection_acceptor
    rtype_server_games_module
  PRIVATE
    rtype_server_constants
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_lobby_module)
