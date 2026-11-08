if(NOT RTYPE_MODULE_DEBUG)
  return()
endif()

add_library(rtype_client_debug_module STATIC
  ClientDebugModule.cpp
  Lagometer.cpp
  LagometerPainting.cpp
  LagometerText.cpp
  LagometerToggle.cpp
)

target_include_directories(rtype_client_debug_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_debug_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_asset_library
    rtype_client_connection_module
    rtype_client_interface
    rtype_client_interface_canvas
    rtype_client_settings_values
    rtype_client_window_display
    rtype_client_window_loop
    rtype_engine_events
    rtype_engine_install_context
    rtype_engine_modules
    rtype_engine_time
    rtype_network_statistics
  PRIVATE
    rtype_client_background
    rtype_client_outline
    rtype_client_constants
)

rtype_enable_warnings(rtype_client_debug_module)
