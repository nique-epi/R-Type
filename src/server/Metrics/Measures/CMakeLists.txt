add_library(rtype_server_metrics_measures STATIC GamesMeasures.cpp TickDurations.cpp)

target_include_directories(rtype_server_metrics_measures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_metrics_measures
  PUBLIC
    rtype_engine_systems
    rtype_network_statistics_round_trip
    rtype_server_games_module
    rtype_server_metrics_contract
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_metrics_measures)
