add_library(rtype_client_options_descriptions STATIC OptionsDescriptions.cpp)

target_include_directories(rtype_client_options_descriptions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_descriptions
  PUBLIC
    rtype_client_key_bindings
    rtype_client_slider
    rtype_client_screen_assets
    rtype_client_settings
    rtype_client_window
    rtype_client_window_layout
  PRIVATE
    rtype_client_dropdown
    rtype_client_key_slot
    rtype_client_window_size_text
)

rtype_enable_warnings(rtype_client_options_descriptions)
