add_library(rtype_network_statistics STATIC
  CountingDatagramSocket.cpp
  MessageTypeText.cpp
  TrafficCounters.cpp
  TrafficCsvFile.cpp
  TrafficCsvRows.cpp
  TrafficLine.cpp
  TrafficRecorder.cpp
  TrafficTotals.cpp
)

target_include_directories(rtype_network_statistics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_statistics
  PUBLIC
    rtype_logging
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_statistics)
