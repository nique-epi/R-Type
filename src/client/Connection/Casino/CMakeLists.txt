add_library(rtype_client_connection_casino STATIC MasterCasinoLink.cpp)

target_include_directories(rtype_client_connection_casino PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_casino
  PUBLIC
    rtype_concurrency_latest_value
    rtype_network_master_messages
    rtype_network_master_routes
  PRIVATE
    rtype_network_exceptions
    rtype_network_http_api
    rtype_network_master_client
)

rtype_enable_warnings(rtype_client_connection_casino)
