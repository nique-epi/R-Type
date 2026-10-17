add_library(rtype_server_datagram_sorter STATIC DatagramSorter.cpp)

target_include_directories(rtype_server_datagram_sorter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_datagram_sorter
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_server_datagram_sorter)
