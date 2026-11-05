add_library(rtype_server_supervisor_lobby_module STATIC ServerSupervisorLobbyModule.cpp)

target_include_directories(rtype_server_supervisor_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervisor_lobby_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_engine_time
    rtype_network_handshake_cookies
    rtype_network_transport
    rtype_server_network_module
    rtype_server_supervised_games_module
  PRIVATE
    rtype_network_connection_challenge
    rtype_network_connection_redirect
    rtype_network_connection_refused
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_server_constants
    rtype_server_lobby_module
)

rtype_enable_warnings(rtype_server_supervisor_lobby_module)
