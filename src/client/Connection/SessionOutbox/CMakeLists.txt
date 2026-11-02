add_library(rtype_client_session_outbox STATIC SessionOutbox.cpp)

target_include_directories(rtype_client_session_outbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session_outbox
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_protocol
    rtype_network_transport
    rtype_network_datagram_codec
    rtype_network_heartbeat
)

rtype_enable_warnings(rtype_client_session_outbox)
