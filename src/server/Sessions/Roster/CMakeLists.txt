add_library(rtype_server_sessions_roster STATIC
  ArrivalLedger.cpp
  ConnectedPlayers.cpp
  PlayerRoster.cpp
  SessionDatagram.cpp
)

target_include_directories(rtype_server_sessions_roster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_sessions_roster
  PUBLIC
    rtype_engine_time
    rtype_game_components
    rtype_identifiers
    rtype_network_protocol
    rtype_network_statistics_round_trip
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_sequence_order
)

rtype_enable_warnings(rtype_server_sessions_roster)
