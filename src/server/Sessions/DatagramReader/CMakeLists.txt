add_library(rtype_server_datagram_reader STATIC DatagramReader.cpp)

target_include_directories(rtype_server_datagram_reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_datagram_reader
  PUBLIC
    rtype_logging_throttled_log
    rtype_network_transport
    rtype_server_player_roster
    rtype_server_sessions
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_server_datagram_reader)
