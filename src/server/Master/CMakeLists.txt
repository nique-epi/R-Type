add_library(rtype_server_master_module STATIC
  HeartbeatJob.cpp
  MasterMessages.cpp
  ServerMasterModule.cpp
)

target_include_directories(rtype_server_master_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_server_master_module PUBLIC RTYPE_GAME_VERSION="${PROJECT_VERSION}")

target_link_libraries(rtype_server_master_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_logging
    rtype_network_master_client
    rtype_network_master_messages
    rtype_server_games_module
    rtype_server_metrics_report
    rtype_server_network_module
  PRIVATE
    rtype_concurrency_interruptible_sleep
    rtype_engine_time
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_wire_types
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_master_module)
