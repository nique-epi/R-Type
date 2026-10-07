add_library(rtype_server_datagram_relay STATIC DatagramRelay.cpp)

target_include_directories(rtype_server_datagram_relay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_datagram_relay
  PUBLIC
    rtype_engine_bounded_queue
    rtype_network_transport
)

rtype_enable_warnings(rtype_server_datagram_relay)
