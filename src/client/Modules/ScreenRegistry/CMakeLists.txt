add_library(rtype_client_screen_registry STATIC ScreenBuilding.cpp)

target_include_directories(rtype_client_screen_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_registry
  PUBLIC
    rtype_client_assets_module
    rtype_client_audio_contract
    rtype_client_entity_messages
    rtype_client_exceptions
    rtype_client_interface
    rtype_client_menu
    rtype_client_pilots
    rtype_client_screen_interfaces
    rtype_client_scrolling_background
    rtype_client_session
    rtype_client_settings_module
    rtype_client_window_module
    rtype_network_transport
  PRIVATE
    rtype_client_dialog_screen
    rtype_client_interface_canvas
    rtype_client_loading_screen
    rtype_client_platform
)

rtype_enable_warnings(rtype_client_screen_registry)
