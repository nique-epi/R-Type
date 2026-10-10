add_library(rtype_server_application STATIC ServerApplication.cpp)

target_include_directories(rtype_server_application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_application
  PUBLIC
    rtype_engine_bounded_queue
    rtype_engine_system_clock
    rtype_logging
    rtype_network_context
    rtype_network_transport
    rtype_network_udp_socket
    rtype_server_datagram_relay
    rtype_server_tick_loop
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_application)
