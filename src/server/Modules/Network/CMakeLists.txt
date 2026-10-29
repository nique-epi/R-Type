add_library(rtype_server_network_module STATIC ServerNetworkModule.cpp TickDatagrams.cpp)

target_include_directories(rtype_server_network_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_network_module
  PUBLIC
    rtype_engine_application
    rtype_logging
    rtype_network_transport
    rtype_server_application
    rtype_server_stop_request
  PRIVATE
    rtype_network_constants
    rtype_server_constants
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_network_module)
