if(NOT RTYPE_MODULE_MASTER_LINK)
  return()
endif()

add_library(rtype_server_master_module STATIC
  HeartbeatJob.cpp
  LastProblem.cpp
  MasterMessages.cpp
  MasterProbeDesk.cpp
  ResultsCourier.cpp
  RetryBackoff.cpp
  ServerMasterModule.cpp
)

target_include_directories(rtype_server_master_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_master_module
  PUBLIC
    rtype_engine_commands
    rtype_concurrency_latest_value
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
    rtype_network_http_api
    rtype_network_master_client
    rtype_network_master_messages
    rtype_network_master_probe
    rtype_network_master_validators
    rtype_server_information_module
    rtype_server_supervised_games_module
    rtype_server_network_module
    rtype_server_storage_database
  PRIVATE
    rtype_concurrency_interruptible_sleep
    rtype_logging_path_text
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_transport
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_master_module)
