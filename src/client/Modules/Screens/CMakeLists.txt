add_library(rtype_client_screens_module STATIC ClientScreensModule.cpp ClientScreens.cpp ScreenCatalog.cpp)

target_include_directories(rtype_client_screens_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screens_module
  PUBLIC
    rtype_client_asset_library
    rtype_client_connection
    rtype_client_game_window
    rtype_client_interface
    rtype_client_music_player
    rtype_client_replication_interfaces
    rtype_client_screen_stack
    rtype_client_scrolling_background
    rtype_client_server_connection
    rtype_client_settings
    rtype_client_sound_player
    rtype_client_soundtrack
    rtype_client_window
    rtype_client_window_module
    rtype_engine_events
    rtype_engine_modules
    rtype_network_transport
  PRIVATE
    rtype_client_connection_screen
    rtype_client_constants
    rtype_client_end_screen
    rtype_client_entity_layer
    rtype_client_game_screen
    rtype_client_interface_canvas
    rtype_client_intro_screen
    rtype_client_loading_screen
    rtype_client_lobby_screen
    rtype_client_localized_key_name
    rtype_client_module_constants
    rtype_client_options_screen
    rtype_client_platform
    rtype_client_player_preferences
    rtype_client_quit_confirmation_screen
    rtype_client_screen_assets
    rtype_client_title_screen
)

rtype_enable_warnings(rtype_client_screens_module)
