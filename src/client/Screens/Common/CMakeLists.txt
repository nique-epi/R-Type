add_library(rtype_client_common_screens STATIC
  DialogScreen.cpp
  LoadingScreen.cpp
)

target_include_directories(rtype_client_common_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_common_screens
  PUBLIC
    SFML::Graphics
    SFML::System
    SFML::Window
    rtype_client_application
    rtype_client_asset_library
    rtype_client_asset_loading
    rtype_client_interface
    rtype_client_menu
    rtype_client_widgets
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_game_components
)

rtype_enable_warnings(rtype_client_common_screens)
