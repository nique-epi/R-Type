add_library(rtype_server_modules STATIC GameLineRule.cpp GameProcessModules.cpp SupervisorModules.cpp)

target_include_directories(rtype_server_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_modules
  PUBLIC
    rtype_engine_application
    rtype_engine_commands
    rtype_logging
    rtype_server_games_module
    rtype_server_content_module
    rtype_server_information_module
    rtype_server_launch_options
    rtype_server_lobby_module
    rtype_server_logger_module
    rtype_server_master_module
    rtype_server_metrics_module
    rtype_server_network_module
    rtype_server_platform
    rtype_server_storage_module
    rtype_server_supervised_games_module
    rtype_server_supervisor_link_module
    rtype_server_supervisor_lobby_module
  PRIVATE
    rtype_engine_paths
    rtype_server_processes
    rtype_engine_system_clock
    rtype_game_entity_types
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_modules)
