add_library(rtype_network_byte_writer STATIC ByteWriter.cpp)

target_include_directories(rtype_network_byte_writer
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(rtype_network_byte_writer PUBLIC rtype_network_exceptions)

rtype_enable_warnings(rtype_network_byte_writer)
