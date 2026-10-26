add_library(rtype_client_game_screen STATIC GameScreen.cpp)

target_include_directories(rtype_client_game_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_screen
  PUBLIC
    rtype_client_connection
    rtype_client_entity_replicator
    rtype_client_mission_banner
    rtype_client_player_commands
    rtype_client_player_input_pacer
    rtype_client_drawing
    rtype_client_score_board
    rtype_client_screen_assets
    rtype_engine
    rtype_logging
    SFML::Graphics
  PRIVATE
    rtype_client_animation_system
    rtype_client_ground_effect_system
    rtype_client_hit_flash_system
    rtype_client_mission_start
    rtype_client_player_preferences
    rtype_client_settings
    rtype_client_sprite_effect_system
    rtype_client_status_band
)

rtype_enable_warnings(rtype_client_game_screen)
