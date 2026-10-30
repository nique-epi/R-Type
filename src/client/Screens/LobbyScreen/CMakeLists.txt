add_library(rtype_client_lobby_screen STATIC LobbyScreen.cpp)

target_include_directories(rtype_client_lobby_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lobby_screen
  PUBLIC
    rtype_client_connection
    rtype_client_help_walkthrough
    rtype_client_menu
    rtype_client_interface
    rtype_client_screen_assets
    rtype_client_window
    SFML::Graphics
  PRIVATE
    rtype_client_hangar_bay
    rtype_client_menu_builder
    rtype_client_pilot_color
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_lobby_screen)
