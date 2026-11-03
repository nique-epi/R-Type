add_library(rtype_client_options_rows STATIC OptionsRows.cpp)

target_include_directories(rtype_client_options_rows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_rows
  PUBLIC
    rtype_client_input
    rtype_client_row_control
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_window_module
  PRIVATE
    rtype_client_choice_selector
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_slider
    rtype_client_switch
)

rtype_enable_warnings(rtype_client_options_rows)
