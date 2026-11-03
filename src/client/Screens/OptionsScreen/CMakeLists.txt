add_library(rtype_client_options_screen STATIC OptionsScreen.cpp)

target_include_directories(rtype_client_options_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_screen
  PUBLIC
    SFML::Graphics
    rtype_client_interface
    rtype_client_menu
    rtype_client_options_tabs
    rtype_client_reset_dialog
    rtype_client_saved_stamp
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_window_module
  PRIVATE
    rtype_client_dropdown
    rtype_client_window_size_chart
)

rtype_enable_warnings(rtype_client_options_screen)
