add_subdirectory(Display)
add_subdirectory(Loop)

add_library(rtype_client_window_module STATIC
  ClientWindowModule.cpp
)

target_include_directories(rtype_client_window_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window_module
  PUBLIC
    rtype_client_settings_values
    rtype_client_window_display
    rtype_client_window_loop
    rtype_engine_install_context
    rtype_engine_modules
  PRIVATE
    SFML::Graphics
    SFML::Window
    rtype_client_constants
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_window_module)
