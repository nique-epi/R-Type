add_library(rtype_server_lobby_state_announcer STATIC LobbyStateAnnouncer.cpp)

target_include_directories(rtype_server_lobby_state_announcer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_lobby_state_announcer
  PUBLIC
    rtype_network_lobby_state
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_server_constants
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_lobby_state_announcer)
