add_library(rtype_client_application STATIC Application.cpp)

target_include_directories(rtype_client_application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_application
  PUBLIC
    rtype_client_asset_library
    rtype_client_connection
    rtype_client_game_window
    rtype_client_network_thread
    rtype_client_interface
    rtype_client_window
    rtype_client_screen_stack
    rtype_client_platform
    rtype_client_scrolling_background
    rtype_client_server_connection
    rtype_client_settings_file
    rtype_engine_system_clock
  PRIVATE
    rtype_client_connection_screen
    rtype_client_entity_layer
    rtype_client_game_screen
    rtype_client_interface_canvas
    rtype_client_loading_screen
    rtype_client_lobby_screen
    rtype_client_localized_key_name
    rtype_client_options_screen
    rtype_client_quit_confirmation_screen
    rtype_client_screens
    rtype_client_title_screen
)

rtype_enable_warnings(rtype_client_application)
