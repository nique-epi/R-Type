add_library(rtype_client_menus_destinations STATIC DestinationScreen.cpp)

target_include_directories(rtype_client_menus_destinations PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_destinations
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_sector_map_interfaces
    rtype_client_widgets
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_interface_pixel
    rtype_client_kopecks_painting
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_menus_destinations)
