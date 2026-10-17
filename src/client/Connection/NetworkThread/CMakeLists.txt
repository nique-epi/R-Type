add_library(rtype_client_network_thread STATIC NetworkThread.cpp)

target_include_directories(rtype_client_network_thread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_network_thread
  PUBLIC
    rtype_client_connection
    rtype_concurrency_bounded_queue
    rtype_network_context
    rtype_network_datagram_relay
    rtype_network_transport
    rtype_network_udp_socket
)

rtype_enable_warnings(rtype_client_network_thread)
