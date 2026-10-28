add_library(rtype_server_modules STATIC ServerModules.cpp)

target_include_directories(rtype_server_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_modules
  PUBLIC
    rtype_engine_application
    rtype_logging
    rtype_server_games_module
    rtype_server_launch_options
    rtype_server_lobby_module
    rtype_server_logger_module
    rtype_server_module_constants
    rtype_server_network_module
    rtype_server_stop_request
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_modules)
