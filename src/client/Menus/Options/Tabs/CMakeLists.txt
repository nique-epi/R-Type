add_library(rtype_client_options_tabs STATIC
  OptionsDescriptions.cpp
  OptionsRows.cpp
  OptionsTabs.cpp
)

target_include_directories(rtype_client_options_tabs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_tabs
  PUBLIC
    rtype_client_audio_contract_sounds
    rtype_client_input_keys
    rtype_client_menu
    rtype_client_row_control
    rtype_client_settings_module
    rtype_client_settings_values
    rtype_client_slider
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    SFML::Window
    rtype_client_choice_selector
    rtype_client_common_screens
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_link_text
    rtype_client_menu_builder
    rtype_client_switch
    rtype_client_text_measures
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_options_tabs)
