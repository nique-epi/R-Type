add_subdirectory(ByteWriter)
add_subdirectory(ByteReader)
add_subdirectory(ByteText)

add_library(rtype_network_serialization INTERFACE)
target_link_libraries(rtype_network_serialization
  INTERFACE
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_byte_text
)
