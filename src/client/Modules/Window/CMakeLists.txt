add_library(rtype_client_window_module STATIC ClientWindowModule.cpp EventListeners.cpp FrameLoop.cpp)

target_include_directories(rtype_client_window_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_window_module
  PUBLIC
    SFML::Window
    rtype_client_game_window
    rtype_client_settings
    rtype_engine_application
    rtype_engine_events
    rtype_engine_modules
    rtype_engine_time
  PRIVATE
    rtype_client_constants
    rtype_client_module_constants
)

rtype_enable_warnings(rtype_client_window_module)
