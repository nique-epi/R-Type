add_library(rtype_server_lobby_module STATIC LobbyModule.cpp)

target_include_directories(rtype_server_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_lobby_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_server_application
    rtype_server_application_interfaces
    rtype_server_connection_acceptor
    rtype_server_game_list
  PRIVATE
    rtype_engine_time
    rtype_server_constants
    rtype_server_instances
)

rtype_enable_warnings(rtype_server_lobby_module)
