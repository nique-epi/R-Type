add_subdirectory(Cookies)
add_subdirectory(RateLimit)

add_library(rtype_network_handshake INTERFACE)

target_link_libraries(rtype_network_handshake
  INTERFACE
    rtype_network_handshake_cookies
    rtype_network_handshake_rate_limit
)
