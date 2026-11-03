add_library(rtype_client_game_screen STATIC GameScreen.cpp)

target_include_directories(rtype_client_game_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_screen
  PUBLIC
    SFML::Graphics
    rtype_client_connection
    rtype_client_departure_feed
    rtype_client_drawing
    rtype_client_entities
    rtype_client_entity_messages
    rtype_client_input
    rtype_client_mission_banner
    rtype_client_pilots
    rtype_client_prediction
    rtype_client_screen_assets
    rtype_client_silence_warning
    rtype_engine
    rtype_logging
  PRIVATE
    rtype_client_animation_system
    rtype_client_ground_effect_system
    rtype_client_hit_flash_system
    rtype_client_mission_start
    rtype_client_player_preferences
    rtype_client_settings_module
    rtype_client_ship_flight_system
    rtype_client_sprite_effect_system
    rtype_client_status_band
)

rtype_enable_warnings(rtype_client_game_screen)
