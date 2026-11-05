add_library(rtype_client_session STATIC
  OrderedInbox.cpp
  SessionAcknowledgements.cpp
  SessionDeparture.cpp
  SessionInbound.cpp
  SessionObservers.cpp
  SessionOutbox.cpp
  RoundTripWatch.cpp
)

target_include_directories(rtype_client_session PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session
  PUBLIC
    rtype_client_transport
    rtype_engine_server_clock
    rtype_engine_time
    rtype_logging
    rtype_network_lobby_state
    rtype_network_message_dispatcher
    rtype_network_ordered_message
    rtype_network_protocol
    rtype_network_round_trips
    rtype_network_statistics_round_trip
    rtype_network_reliability
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_exceptions
    rtype_network_heartbeat
)

rtype_enable_warnings(rtype_client_session)
