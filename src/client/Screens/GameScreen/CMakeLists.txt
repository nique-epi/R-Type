add_library(rtype_client_game_screen STATIC GameScreen.cpp)

target_include_directories(rtype_client_game_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_screen
  PUBLIC
    rtype_client_screen_stack
    SFML::Graphics
  PRIVATE
    rtype_client_screens
)

rtype_enable_warnings(rtype_client_game_screen)
