add_library(rtype_network_cookie_issuer STATIC CookieIssuer.cpp)

target_include_directories(rtype_network_cookie_issuer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_cookie_issuer
  PUBLIC
    rtype_network_handshake
    rtype_network_transport
  PRIVATE
    rtype_network_byte_writer
)

rtype_enable_warnings(rtype_network_cookie_issuer)
