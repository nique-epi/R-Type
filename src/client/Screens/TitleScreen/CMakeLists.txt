add_library(rtype_client_title_screen STATIC TitleScreen.cpp)

target_include_directories(rtype_client_title_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_title_screen
  PUBLIC
    rtype_client_menu
    rtype_client_interface
    rtype_client_screens
    rtype_client_window
    rtype_client_squadron
    SFML::Graphics
  PRIVATE
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_title_screen)
