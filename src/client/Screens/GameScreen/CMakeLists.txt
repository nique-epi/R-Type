add_library(rtype_client_game_screen STATIC GameScreen.cpp)

target_include_directories(rtype_client_game_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_screen
  PUBLIC
    rtype_client_connection
    rtype_client_player_commands
    rtype_client_player_input_pacer
    rtype_client_rendering_interfaces
    rtype_client_screens
    rtype_engine
    rtype_logging
    SFML::Graphics
  PRIVATE
    rtype_client_entity_sprites
)

rtype_enable_warnings(rtype_client_game_screen)
