add_library(rtype_client_reset_dialog STATIC ResetDialog.cpp)

target_include_directories(rtype_client_reset_dialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_reset_dialog
  PUBLIC
    rtype_client_key_bindings
    rtype_client_settings
    rtype_client_slider
    rtype_client_window
    rtype_client_interface
    rtype_client_menu
    rtype_client_screen_assets
    rtype_engine_time
    SFML::Graphics
  PRIVATE
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_reset_dialog)
