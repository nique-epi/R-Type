add_library(rtype_server_metrics_module STATIC
  GamesMeasures.cpp
  LatestMetrics.cpp
  MetricsLines.cpp
  ServerMetricsModule.cpp
  TickDurations.cpp
)

target_include_directories(rtype_server_metrics_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics_module
  PUBLIC
    rtype_engine_application
    rtype_engine_systems
    rtype_logging
    rtype_server_games_module
  PRIVATE
    rtype_server_constants
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_metrics_module)
