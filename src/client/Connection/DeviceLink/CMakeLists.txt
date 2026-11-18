add_library(rtype_client_connection_device_link STATIC MasterDeviceLinkFlow.cpp)

target_include_directories(rtype_client_connection_device_link PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_device_link
  PUBLIC
    rtype_concurrency_latest_value
  PRIVATE
    rtype_client_connection_master_calls
    rtype_network_exceptions
    rtype_network_http_api
    rtype_network_master_client
)

rtype_enable_warnings(rtype_client_connection_device_link)
