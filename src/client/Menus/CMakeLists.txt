add_subdirectory(Leaderboard)
add_subdirectory(Options)
add_subdirectory(ServerBrowser)
add_subdirectory(Title)

add_library(rtype_client_menus_module STATIC ClientMenusModule.cpp)

target_include_directories(rtype_client_menus_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_module
  PUBLIC
    rtype_client_interface
    rtype_client_screens_module
    rtype_engine_modules
  PRIVATE
    rtype_client_text_measures
    rtype_client_constants
    rtype_client_input_keys
    rtype_client_menus_leaderboard
    rtype_client_menus_server_browser
    rtype_client_options
    rtype_client_title
)

rtype_enable_warnings(rtype_client_menus_module)
