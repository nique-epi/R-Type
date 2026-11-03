add_library(rtype_server_metrics STATIC
  LatestMetrics.cpp
  MetricsLines.cpp
  TickDurations.cpp
)

target_include_directories(rtype_server_metrics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics
  PUBLIC
    rtype_engine_systems
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_metrics)
