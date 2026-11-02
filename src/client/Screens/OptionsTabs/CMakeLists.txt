add_library(rtype_client_options_tabs STATIC OptionsTabs.cpp)

target_include_directories(rtype_client_options_tabs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_tabs
  PUBLIC
    rtype_client_key_bindings
    rtype_client_slider
    rtype_client_key_names
    rtype_client_menu
    rtype_client_screen_assets
    rtype_client_settings_service
    rtype_client_window
  PRIVATE
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_link_text
    rtype_client_menu_builder
    rtype_client_options_descriptions
    rtype_client_options_rows
    rtype_client_window_layout
    rtype_client_window_size_chart
)

rtype_enable_warnings(rtype_client_options_tabs)
