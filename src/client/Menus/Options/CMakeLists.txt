add_library(rtype_client_options STATIC
  OptionsDescriptions.cpp
  OptionsRows.cpp
  OptionsScreen.cpp
  OptionsTabs.cpp
  ResetDialog.cpp
  SavedStamp.cpp
)

target_include_directories(rtype_client_options PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options
  PUBLIC
    SFML::Graphics
    rtype_client_input
    rtype_client_interface
    rtype_client_menu
    rtype_client_row_control
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_slider
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_choice_selector
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_link_text
    rtype_client_menu_builder
    rtype_client_switch
    rtype_client_window_size_chart
)

rtype_enable_warnings(rtype_client_options)
