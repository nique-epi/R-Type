add_library(rtype_network_statistics_counting STATIC
  CountingDatagramSocket.cpp
  TrafficCounters.cpp
  TrafficTotals.cpp
)

target_include_directories(rtype_network_statistics_counting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_statistics_counting
  PUBLIC
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_network_statistics_counting)
