add_library(rtype_client_screens_module STATIC
  ClientScreens.cpp
  ClientScreensModule.cpp
  DialogScreen.cpp
  LoadingScreen.cpp
  OpeningScreen.cpp
  PlayerPreferences.cpp
  ScreenAssets.cpp
  ScreenBuilding.cpp
)

target_include_directories(rtype_client_screens_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screens_module
  PUBLIC
    SFML::Graphics
    rtype_client_application
    rtype_client_assets_module
    rtype_client_audio_contract
    rtype_client_effect_interfaces
    rtype_client_entity_messages
    rtype_client_exceptions
    rtype_client_interface
    rtype_client_menu
    rtype_client_pilots
    rtype_client_rendering_interfaces
    rtype_client_scrolling_background
    rtype_client_session
    rtype_client_settings_module
    rtype_client_widgets
    rtype_client_window_module
    rtype_engine_commands
    rtype_engine_events
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_transport
  PRIVATE
    rtype_client_constants
    rtype_client_interface_canvas
    rtype_client_platform
    rtype_game
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_protocol
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_screens_module)

add_subdirectory(ConnectionScreen)
add_subdirectory(IntroScreen)
add_subdirectory(OptionsRows)
add_subdirectory(OptionsDescriptions)
add_subdirectory(OptionsTabs)
add_subdirectory(SavedStamp)
add_subdirectory(ResetDialog)
add_subdirectory(OptionsScreen)
add_subdirectory(TitleScreen)
add_subdirectory(DialogMenus)
