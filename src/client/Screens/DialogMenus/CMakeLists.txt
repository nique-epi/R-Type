add_library(rtype_client_dialog_menus STATIC DialogMenus.cpp)

target_include_directories(rtype_client_dialog_menus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_dialog_menus
  PUBLIC
    rtype_client_connection
    rtype_client_menu
    rtype_client_screen_interfaces
  PRIVATE
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_dialog_menus)
