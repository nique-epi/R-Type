add_library(rtype_network_compression_sockets STATIC
  CompressingDatagramSocket.cpp
  DatagramCompression.cpp
  DecompressingDatagramSocket.cpp
)

target_include_directories(rtype_network_compression_sockets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_compression_sockets
  PUBLIC
    rtype_network_compression_constants
    rtype_network_compression_huffman
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_network_compression_sockets)
