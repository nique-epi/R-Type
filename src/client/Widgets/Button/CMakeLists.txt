add_library(rtype_client_button STATIC Button.cpp)

target_include_directories(rtype_client_button PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_button
  PUBLIC
    rtype_client_cut_corner_box
    rtype_client_row_control
    rtype_client_sunk_box
    rtype_client_text_field
    rtype_client_widgets
  PRIVATE
    rtype_client_key_cap
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_button)
