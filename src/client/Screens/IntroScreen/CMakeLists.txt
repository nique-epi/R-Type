add_library(rtype_client_intro_screen STATIC IntroScreen.cpp)

target_include_directories(rtype_client_intro_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_screen
  PUBLIC
    rtype_client_camera_shake
    rtype_client_interface
    rtype_client_intro_interfaces
    rtype_client_screen_assets
    rtype_client_synced_clock
    rtype_logging
    SFML::Graphics
  PRIVATE
    rtype_client_intro_cast_builder
    rtype_client_intro_cues
    rtype_client_intro_overlay
    rtype_client_intro_painting
    rtype_client_intro_timeline
    rtype_client_player_preferences
)

rtype_enable_warnings(rtype_client_intro_screen)
