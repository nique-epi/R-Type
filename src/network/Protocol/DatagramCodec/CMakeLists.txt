add_library(rtype_network_datagram_codec STATIC DatagramCodec.cpp)

target_include_directories(rtype_network_datagram_codec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_datagram_codec
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_datagram_codec)
