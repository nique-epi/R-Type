add_library(rtype_network_udp_socket STATIC UdpSocket.cpp)

target_include_directories(rtype_network_udp_socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_udp_socket
  PUBLIC
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_transport
    rtype_network_context
  PRIVATE
    rtype_network_asio
    rtype_logging
)

rtype_enable_warnings(rtype_network_udp_socket)
