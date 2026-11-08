add_subdirectory(Locker)
add_subdirectory(Dock)

add_library(rtype_client_hangar_module STATIC
  ClientHangarModule.cpp
  HangarScreen.cpp
  WonItems.cpp
)

target_include_directories(rtype_client_hangar_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hangar_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_audio_contract_service
    rtype_client_hangar_dock
    rtype_client_hangar_locker
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_waiting_room
    rtype_engine_modules
  PRIVATE
    rtype_client_constants
    rtype_client_interface_pixel
    rtype_client_menu_builder
    rtype_client_pilot_color
    rtype_client_ship_looks
    rtype_client_widgets
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_hangar_module)
