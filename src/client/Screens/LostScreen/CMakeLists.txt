add_library(rtype_client_lost_screen STATIC LostScreen.cpp)

target_include_directories(rtype_client_lost_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lost_screen
  PUBLIC
    rtype_client_connection
    rtype_client_interface
    rtype_client_rendering_interfaces
    rtype_client_screen_interfaces
    rtype_client_widgets
    rtype_client_window
    SFML::Graphics
  PRIVATE
    rtype_client_effect_interfaces
    rtype_client_glowing_call_painting
    rtype_client_held_key
    rtype_client_interface_pixel
    rtype_client_no_signal_card
    rtype_client_screen_off
    rtype_game_components
)

rtype_enable_warnings(rtype_client_lost_screen)
