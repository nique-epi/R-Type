add_library(rtype_client_title_screen STATIC TitleScreen.cpp)

target_include_directories(rtype_client_title_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_title_screen
  PUBLIC
    SFML::Graphics
    rtype_client_interface
    rtype_client_menu
    rtype_client_screens_module
    rtype_client_squadron
    rtype_client_window_module
  PRIVATE
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_title_screen)
