add_library(rtype_client_lobby_module STATIC
  ClientLobbyModule.cpp
  LobbyScreen.cpp
)

target_include_directories(rtype_client_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lobby_module
  PUBLIC
    SFML::Graphics
    rtype_client_handshake
    rtype_client_help_walkthrough
    rtype_client_interface
    rtype_client_menu
    rtype_client_screens_module
    rtype_client_waiting_room
    rtype_client_window_display
    rtype_engine_modules
  PRIVATE
    rtype_client_constants
    rtype_client_hangar_bay
    rtype_client_input_keys
    rtype_client_menu_builder
    rtype_client_pilot_color
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_lobby_module)
