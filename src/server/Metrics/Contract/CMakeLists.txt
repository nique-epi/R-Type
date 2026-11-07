add_library(rtype_server_metrics_contract INTERFACE)

target_include_directories(rtype_server_metrics_contract INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics_contract
  INTERFACE
    rtype_concurrency_latest_value
    rtype_engine_systems
    rtype_engine_time
    rtype_network_statistics
    rtype_network_statistics_round_trip
)
