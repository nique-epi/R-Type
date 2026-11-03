add_library(rtype_client_window_display STATIC
  GameWindow.cpp
  PlayfieldViewport.cpp
  WindowLayout.cpp
  WindowSizeSelection.cpp
  WindowSizeText.cpp
)

target_include_directories(rtype_client_window_display PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window_display
  PUBLIC
    SFML::Graphics
    SFML::System
    SFML::Window
    rtype_client_settings_values
  PRIVATE
    rtype_client_platform
    rtype_game_components
)

rtype_enable_warnings(rtype_client_window_display)
