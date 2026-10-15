add_library(rtype_client_menu_builder STATIC MenuBuilder.cpp)

target_include_directories(rtype_client_menu_builder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menu_builder
  PUBLIC
    rtype_client_menu
    rtype_client_screen_interfaces
)

rtype_enable_warnings(rtype_client_menu_builder)
