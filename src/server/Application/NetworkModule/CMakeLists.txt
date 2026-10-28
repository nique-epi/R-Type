add_library(rtype_server_network_module STATIC NetworkModule.cpp)

target_include_directories(rtype_server_network_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_network_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
    rtype_network_transport
    rtype_server_application
    rtype_server_application_interfaces
    rtype_server_server_link
    rtype_server_stop_signals
  PRIVATE
    rtype_engine_time
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_network_module)
