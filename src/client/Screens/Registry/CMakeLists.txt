add_library(rtype_client_screen_registry STATIC
  ScreenBuilding.cpp
)

target_include_directories(rtype_client_screen_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_registry
  PUBLIC
    rtype_client_application
    rtype_client_asset_library
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
    rtype_client_connection_module
    rtype_client_entity_messages
    rtype_client_exceptions
    rtype_client_interface
    rtype_client_menu
    rtype_client_pilots
    rtype_client_scrolling_background
    rtype_client_settings_module
    rtype_client_settings_values
    rtype_client_waiting_room
    rtype_client_window_display
    rtype_network_transport
  PRIVATE
    SFML::System
    rtype_client_common_screens
    rtype_client_interface_canvas
    rtype_client_platform
)

rtype_enable_warnings(rtype_client_screen_registry)
