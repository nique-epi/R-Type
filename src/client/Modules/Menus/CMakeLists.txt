add_library(rtype_client_menus STATIC ClientMenusModule.cpp OpeningScreen.cpp)

target_include_directories(rtype_client_menus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus
  PUBLIC
    rtype_client_interface
    rtype_client_screen_interfaces
    rtype_client_screen_registry
    rtype_engine_modules
  PRIVATE
    rtype_client_connection_screen
    rtype_client_intro_screen
    rtype_client_localized_key_name
    rtype_client_module_constants
    rtype_client_options_screen
    rtype_client_quit_confirmation_screen
    rtype_client_screen_assets
    rtype_client_title_screen
)

rtype_enable_warnings(rtype_client_menus)
