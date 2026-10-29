add_library(rtype_client_help_walkthrough STATIC HelpWalkthrough.cpp)

target_include_directories(rtype_client_help_walkthrough PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_help_walkthrough
  PUBLIC
    rtype_client_audio_interfaces
    rtype_client_button
    rtype_client_input
    rtype_client_key_bindings
    rtype_client_widgets
    rtype_client_window
    SFML::Window
  PRIVATE
    rtype_client_animation
    rtype_client_box_corners
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_interface_pixel
    rtype_client_key_cap
    rtype_client_pilot_color
    rtype_client_player_shading
    rtype_client_status_band
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_help_walkthrough)
