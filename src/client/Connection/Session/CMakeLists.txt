add_library(rtype_client_session STATIC
  SessionDeparture.cpp
  SessionInbound.cpp
  SessionObservers.cpp
  SessionOutbox.cpp
)

target_include_directories(rtype_client_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session
  PUBLIC
    rtype_client_transport
    rtype_engine_time
    rtype_logging
    rtype_network_lobby_state
    rtype_network_protocol
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_exceptions
    rtype_network_heartbeat
    rtype_network_sequence_order
)

rtype_enable_warnings(rtype_client_session)
