add_library(rtype_server_player_controller STATIC PlayerController.cpp)

target_include_directories(rtype_server_player_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_player_controller
  PUBLIC
    rtype_game
    rtype_server_player_roster
    rtype_server_sessions
  PRIVATE
    rtype_server_ship_input
)

rtype_enable_warnings(rtype_server_player_controller)
