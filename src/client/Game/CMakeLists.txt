add_library(rtype_client_game_module STATIC
  ClientGameModule.cpp
  EndScreen.cpp
  GameScreen.cpp
  HeldKey.cpp
)

target_include_directories(rtype_client_game_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_departure_feed
    rtype_client_drawing
    rtype_client_entities
    rtype_client_entity_messages
    rtype_client_flight_log_interfaces
    rtype_client_input
    rtype_client_interface
    rtype_client_mission_banner
    rtype_client_pilots
    rtype_client_prediction
    rtype_client_screens_module
    rtype_client_session
    rtype_client_silence_warning
    rtype_client_window_module
    rtype_engine
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_animation_system
    rtype_client_constants
    rtype_client_end_choices
    rtype_client_entity_layer
    rtype_client_flight_log_painting
    rtype_client_flight_log_timeline
    rtype_client_ground_effect_system
    rtype_client_hit_flash_system
    rtype_client_interface_pixel
    rtype_client_mission_start
    rtype_client_settings_module
    rtype_client_ship_flight_system
    rtype_client_sprite_effect_system
    rtype_client_status_band
    rtype_client_taunts
)

rtype_enable_warnings(rtype_client_game_module)
