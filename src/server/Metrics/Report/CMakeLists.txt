add_library(rtype_server_metrics_report STATIC LatestMetrics.cpp MetricsLines.cpp)

target_include_directories(rtype_server_metrics_report PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics_report
  PUBLIC
    rtype_engine_systems
    rtype_network_statistics
    rtype_server_metrics_measures
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_metrics_report)
