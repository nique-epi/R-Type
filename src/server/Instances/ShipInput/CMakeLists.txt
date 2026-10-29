add_library(rtype_server_ship_input STATIC ShipInput.cpp)

target_include_directories(rtype_server_ship_input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_ship_input
  PUBLIC
    rtype_game
    rtype_game_ship_control
    rtype_network_player_input
    rtype_server_sessions
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_server_ship_input)
