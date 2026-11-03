add_subdirectory(Measures)
add_subdirectory(Report)

add_library(rtype_server_metrics_module STATIC ServerMetricsModule.cpp)

target_include_directories(rtype_server_metrics_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics_module
  PUBLIC
    rtype_engine_application
    rtype_logging
    rtype_server_games_module
    rtype_server_metrics_measures
    rtype_server_metrics_report
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_metrics_module)
