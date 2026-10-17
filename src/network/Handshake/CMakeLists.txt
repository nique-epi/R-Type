add_library(rtype_network_handshake STATIC SipHash.cpp)

target_include_directories(rtype_network_handshake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_handshake
  PUBLIC
    rtype_network_byte_writer
)

rtype_enable_warnings(rtype_network_handshake)

add_subdirectory(CookieIssuer)
