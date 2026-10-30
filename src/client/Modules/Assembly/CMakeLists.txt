add_library(rtype_client_modules STATIC ClientModules.cpp)

target_include_directories(rtype_client_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_modules
  PUBLIC
    rtype_client_launch_options
    rtype_client_window_module
    rtype_engine_application
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_assets_module
    rtype_client_audio_module
    rtype_client_connection_module
    rtype_client_game
    rtype_client_lobby
    rtype_client_logger_module
    rtype_client_menus
    rtype_client_screen_registry
    rtype_client_screens_module
    rtype_client_settings_module
)

rtype_enable_warnings(rtype_client_modules)
