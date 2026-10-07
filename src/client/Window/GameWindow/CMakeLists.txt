add_library(rtype_client_game_window STATIC GameWindow.cpp)

target_include_directories(rtype_client_game_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_window
  PUBLIC
    SFML::Graphics
    rtype_client_scrolling_background
    rtype_client_window_size_buttons
    rtype_client_window_size_selection
    rtype_engine_system_clock
  PRIVATE
    rtype_client_window
    rtype_client_playfield_viewport
    rtype_game
)

rtype_enable_warnings(rtype_client_game_window)
