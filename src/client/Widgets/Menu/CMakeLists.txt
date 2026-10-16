add_library(rtype_client_menu STATIC Menu.cpp)

target_include_directories(rtype_client_menu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menu
  PUBLIC
    rtype_client_button
    rtype_client_label
    rtype_client_row_control
    rtype_client_tab_strip
    rtype_client_text_field
    rtype_client_widgets
    rtype_client_window
  PRIVATE
    rtype_client_chamfered_box
)

rtype_enable_warnings(rtype_client_menu)
