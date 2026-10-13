add_library(rtype_network_datagram_relay STATIC DatagramRelay.cpp)

target_include_directories(rtype_network_datagram_relay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_datagram_relay
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_network_transport
)

rtype_enable_warnings(rtype_network_datagram_relay)
