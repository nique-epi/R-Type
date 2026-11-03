add_library(rtype_server_storage_module STATIC
  ResultsArchive.cpp
  ResultsDatabase.cpp
  ServerStorageModule.cpp
)

target_include_directories(rtype_server_storage_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_storage_module
  PUBLIC
    rtype_concurrency
    rtype_engine_application
    rtype_engine_commands
    rtype_logging
    rtype_network_leaderboard_answer
    rtype_network_leaderboard_request
    rtype_network_transport
    rtype_server_network_module
    rtype_storage
  PRIVATE
    rtype_engine_paths
    rtype_network_constants
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_storage_module)
