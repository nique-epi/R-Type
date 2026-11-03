add_subdirectory(Common)
add_subdirectory(Registry)

add_library(rtype_client_screens_module STATIC
  ClientScreens.cpp
  ClientScreensModule.cpp
  OpeningScreen.cpp
  PlayerPreferences.cpp
  ScreenAssets.cpp
)

target_include_directories(rtype_client_screens_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screens_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_asset_library
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
    rtype_client_common_screens
    rtype_client_connection_leaderboard
    rtype_client_connection_module
    rtype_client_effect_interfaces
    rtype_client_entity_messages
    rtype_client_handshake
    rtype_client_interface
    rtype_client_pilots
    rtype_client_rendering_interfaces
    rtype_client_screen_registry
    rtype_client_scrolling_background
    rtype_client_settings_module
    rtype_client_settings_values
    rtype_client_waiting_room
    rtype_client_window_display
    rtype_client_window_loop
    rtype_engine_command_settings
    rtype_engine_events
    rtype_engine_install_context
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_transport
  PRIVATE
    rtype_client_asset_loading
    rtype_client_constants
    rtype_client_sprites
    rtype_engine_exceptions
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_screens_module)
