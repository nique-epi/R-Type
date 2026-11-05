add_library(rtype_network_compression_huffman STATIC
  BitReader.cpp
  BitWriter.cpp
  HuffmanCodec.cpp
  HuffmanTable.cpp
)

target_include_directories(rtype_network_compression_huffman PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_compression_huffman PUBLIC rtype_network_compression_constants)

rtype_enable_warnings(rtype_network_compression_huffman)
