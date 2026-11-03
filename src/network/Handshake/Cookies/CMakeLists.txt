add_library(rtype_network_handshake_cookies STATIC
  CookieIssuer.cpp
  SipHash.cpp
)

target_include_directories(rtype_network_handshake_cookies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_handshake_cookies
  PUBLIC
    rtype_network_byte_writer
    rtype_network_transport
)

rtype_enable_warnings(rtype_network_handshake_cookies)
