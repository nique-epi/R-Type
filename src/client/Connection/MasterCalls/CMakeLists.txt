add_library(rtype_client_connection_master_calls INTERFACE)

target_include_directories(rtype_client_connection_master_calls INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_master_calls
  INTERFACE
    rtype_concurrency_latest_value
    rtype_network_exceptions
)
