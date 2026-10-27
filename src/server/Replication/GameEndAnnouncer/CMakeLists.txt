add_library(rtype_server_game_end_announcer STATIC GameEndAnnouncer.cpp)

target_include_directories(rtype_server_game_end_announcer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_end_announcer
  PUBLIC
    rtype_network_game_ended
    rtype_server_constants
    rtype_server_entity_spawn_sender
    rtype_server_player_roster
  PRIVATE
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_game_end_announcer)
