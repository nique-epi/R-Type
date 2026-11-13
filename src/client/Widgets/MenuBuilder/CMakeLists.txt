add_library(rtype_client_menu_builder STATIC MenuBuilder.cpp)

target_include_directories(rtype_client_menu_builder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menu_builder
  PUBLIC
    rtype_client_application
    rtype_client_menu
  PRIVATE
    rtype_client_key_cap
    rtype_client_panel_layouts
    rtype_client_dialog_layouts
    rtype_client_text_measures
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_menu_builder)
