find_package(asio CONFIG REQUIRED)

add_library(rtype_network_asio INTERFACE)

target_link_libraries(rtype_network_asio INTERFACE asio::asio)

target_compile_definitions(rtype_network_asio
  INTERFACE
    ASIO_NO_DEPRECATED
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
)

add_library(rtype_network_constants INTERFACE)

target_include_directories(rtype_network_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Exceptions)
add_subdirectory(NetworkContext)
add_subdirectory(Serialization)
add_subdirectory(Protocol)
add_subdirectory(Transport)
add_subdirectory(Handshake)

add_library(rtype_network INTERFACE)

target_include_directories(rtype_network INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network
  INTERFACE
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_context
    rtype_network_serialization
    rtype_network_wire_types
    rtype_network_sequence_order
    rtype_network_world_state_codec
    rtype_network_datagram_codec
    rtype_network_messages
    rtype_network_message_dispatcher
    rtype_network_transport
    rtype_network_udp_socket
    rtype_network_datagram_relay
    rtype_network_handshake
    rtype_network_cookie_issuer
)
