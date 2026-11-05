add_library(rtype_client_transport STATIC
  DatagramRoute.cpp
  DatagramRouter.cpp
  NetworkThread.cpp
)

target_include_directories(rtype_client_transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_transport
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_network_compression
    rtype_network_context
    rtype_network_datagram_relay
    rtype_network_simulator
    rtype_network_statistics
    rtype_network_transport
)

rtype_enable_warnings(rtype_client_transport)
