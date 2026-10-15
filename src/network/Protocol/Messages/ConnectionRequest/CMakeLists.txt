add_library(rtype_network_connection_request STATIC ConnectionRequest.cpp)

target_include_directories(rtype_network_connection_request PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_connection_request
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_wire_types
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_connection_request)
