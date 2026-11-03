add_library(rtype_client_menus_module STATIC ClientMenusModule.cpp)

target_include_directories(rtype_client_menus_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_module
  PUBLIC
    rtype_client_interface
    rtype_client_screens_module
    rtype_engine_modules
  PRIVATE
    rtype_client_connection_screen
    rtype_client_constants
    rtype_client_dialog_menus
    rtype_client_input
    rtype_client_intro_screen
    rtype_client_options_screen
    rtype_client_title_screen
)

rtype_enable_warnings(rtype_client_menus_module)
