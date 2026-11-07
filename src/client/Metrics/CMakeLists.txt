if(NOT RTYPE_MODULE_METRICS)
  return()
endif()

add_library(rtype_client_metrics_module STATIC ClientMetricsModule.cpp)

target_include_directories(rtype_client_metrics_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_metrics_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_time
    rtype_network_statistics
  PRIVATE
    rtype_client_constants
)

rtype_enable_warnings(rtype_client_metrics_module)
