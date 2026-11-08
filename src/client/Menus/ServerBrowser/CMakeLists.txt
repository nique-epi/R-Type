add_library(rtype_client_menus_server_browser STATIC
  GamePanel.cpp
  HangarStar.cpp
  ServerMapChoice.cpp
  ServerMapPainting.cpp
  ServerMapScreen.cpp
)

target_include_directories(rtype_client_menus_server_browser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_server_browser
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_sector_map_interfaces
    rtype_client_server_list
    rtype_client_star_navigation
    rtype_client_widgets
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_button
    rtype_client_cut_corner_box
    rtype_client_ink_tag
    rtype_client_interface_pixel
    rtype_client_label_placement
    rtype_client_menu_builder
    rtype_client_pixel_circle
    rtype_client_ringed_planet
    rtype_client_sector_layout
    rtype_client_sector_map_painting
)

rtype_enable_warnings(rtype_client_menus_server_browser)
