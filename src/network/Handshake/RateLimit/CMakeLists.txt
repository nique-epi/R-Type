add_library(rtype_network_handshake_rate_limit STATIC
  AllowanceSpending.cpp
  RequestAllowances.cpp
)

target_include_directories(rtype_network_handshake_rate_limit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_handshake_rate_limit
  PUBLIC
    rtype_network_constants
    rtype_network_handshake_cookies
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_network_byte_writer
)

rtype_enable_warnings(rtype_network_handshake_rate_limit)
