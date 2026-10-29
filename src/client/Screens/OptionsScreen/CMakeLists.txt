add_library(rtype_client_options_screen STATIC OptionsScreen.cpp)

target_include_directories(rtype_client_options_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_screen
  PUBLIC
    rtype_client_key_names
    rtype_client_menu
    rtype_client_menu_builder
    rtype_client_interface
    rtype_client_screen_assets
    rtype_client_settings
    rtype_client_settings_service
    rtype_client_window
    rtype_client_window_layout
    SFML::Graphics
  PRIVATE
    rtype_client_choice_selector
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_link_text
    rtype_client_slider
    rtype_client_switch
    rtype_client_window_size_chart
    rtype_client_window_size_text
)

rtype_enable_warnings(rtype_client_options_screen)
