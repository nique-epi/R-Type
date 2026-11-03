add_library(rtype_client_lobby_module STATIC
  ClientLobbyModule.cpp
  LobbyScreen.cpp
)

target_include_directories(rtype_client_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lobby_module
  PUBLIC
    SFML::Graphics
    rtype_client_help_walkthrough
    rtype_client_interface
    rtype_client_menu
    rtype_client_screen_assets
    rtype_client_screen_registry
    rtype_client_session
    rtype_client_window_module
    rtype_engine_modules
  PRIVATE
    rtype_client_constants
    rtype_client_hangar_bay
    rtype_client_input
    rtype_client_menu_builder
    rtype_client_pilot_color
    rtype_client_player_preferences
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_lobby_module)
