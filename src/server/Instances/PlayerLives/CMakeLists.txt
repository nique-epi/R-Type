add_library(rtype_server_player_lives STATIC PlayerLives.cpp)

target_include_directories(rtype_server_player_lives PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_player_lives
  PUBLIC
    rtype_game
    rtype_server_sessions
)

rtype_enable_warnings(rtype_server_player_lives)
