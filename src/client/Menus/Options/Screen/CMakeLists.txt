add_library(rtype_client_options_screen STATIC
  OptionsScreen.cpp
  ResetDialog.cpp
  SavedStamp.cpp
)

target_include_directories(rtype_client_options_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_screen
  PUBLIC
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_input_keys
    rtype_client_interface
    rtype_client_menu
    rtype_client_options_tabs
    rtype_client_settings_module
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    SFML::Graphics
    rtype_client_common_screens
    rtype_client_dropdown
    rtype_client_menu_builder
    rtype_client_window_size_chart
)

rtype_enable_warnings(rtype_client_options_screen)
