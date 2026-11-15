add_library(rtype_client_menus_server_browser STATIC
  ChosenGameConnection.cpp
  GamePanel.cpp
  NewGameDialog.cpp
  ServerMapChoice.cpp
  ServerMapPainting.cpp
  ServerMapScreen.cpp
  SessionEndWatch.cpp
)

target_include_directories(rtype_client_menus_server_browser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_server_browser
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_menu
    rtype_client_handshake
    rtype_client_screens_module
    rtype_client_sector_map_interfaces
    rtype_client_server_list
    rtype_client_star_navigation
    rtype_client_widgets
    rtype_client_window_display
    rtype_engine_time
    rtype_network_master_validators
  PRIVATE
    rtype_client_button
    rtype_client_choice_selector
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_ink_tag
    rtype_client_interface_pixel
    rtype_client_label_placement
    rtype_client_menu_builder
    rtype_client_ringed_planet
    rtype_client_row_control
    rtype_client_sector_layout
    rtype_client_sector_map_painting
    rtype_client_switch
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_menus_server_browser)
