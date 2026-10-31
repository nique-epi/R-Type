add_library(rtype_client_dialog_screen STATIC DialogScreen.cpp)

target_include_directories(rtype_client_dialog_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_dialog_screen
  PUBLIC
    rtype_client_menu
    rtype_client_interface
    rtype_client_screen_interfaces
    rtype_client_window
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_dialog_screen)
