add_library(rtype_client_game_module STATIC
  ClientGameModule.cpp
  EndScreen.cpp
  GameScreen.cpp
  HeldKey.cpp
  PilotNameTags.cpp
)

target_include_directories(rtype_client_game_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_connection_module
    rtype_client_departure_feed
    rtype_client_drawing
    rtype_client_flight_log_interfaces
    rtype_client_handshake
    rtype_client_input
    rtype_client_interface
    rtype_client_mission_banner
    rtype_client_prediction
    rtype_client_replication
    rtype_client_screens_module
    rtype_client_silence_warning
    rtype_client_waiting_room
    rtype_client_window_display
    rtype_engine
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_animation_system
    rtype_client_constants
    rtype_client_effect_targets
    rtype_client_end_choices
    rtype_client_entity_layer
    rtype_client_flight_log_painting
    rtype_client_flight_log_timeline
    rtype_client_ground_effect_system
    rtype_client_hit_flash_system
    rtype_client_interface_pixel
    rtype_client_mission_start
    rtype_client_pilot_color
    rtype_client_pilot_name
    rtype_client_settings_values
    rtype_client_ship_flight_system
    rtype_client_sprite_effect_system
    rtype_client_status_band
    rtype_client_taunts
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_game_module)
