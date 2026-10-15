add_library(rtype_client_options_screen STATIC OptionsScreen.cpp)

target_include_directories(rtype_client_options_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_options_screen
  PUBLIC
    rtype_client_menu
    rtype_client_rendering_interfaces
    rtype_client_screens
    rtype_client_window
    SFML::Graphics
  PRIVATE
    rtype_client_game_window
    rtype_client_menu_builder
)

rtype_enable_warnings(rtype_client_options_screen)
