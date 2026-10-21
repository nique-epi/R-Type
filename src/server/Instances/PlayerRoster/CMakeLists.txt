add_library(rtype_server_player_roster STATIC PlayerRoster.cpp)

target_include_directories(rtype_server_player_roster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_player_roster
  PUBLIC
    rtype_network_protocol
    rtype_server_connected_players
    rtype_server_instances
)

rtype_enable_warnings(rtype_server_player_roster)
