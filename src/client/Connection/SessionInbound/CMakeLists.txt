add_library(rtype_client_session_inbound STATIC SessionInbound.cpp)

target_include_directories(rtype_client_session_inbound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session_inbound
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_protocol
    rtype_network_transport
    rtype_network_datagram_codec
  PRIVATE
    rtype_network_exceptions
    rtype_network_sequence_order
)

rtype_enable_warnings(rtype_client_session_inbound)
