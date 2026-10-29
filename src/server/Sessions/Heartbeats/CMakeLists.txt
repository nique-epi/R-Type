add_library(rtype_server_heartbeats STATIC Heartbeats.cpp)

target_include_directories(rtype_server_heartbeats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_heartbeats
  PUBLIC
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_network_heartbeat
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_heartbeats)
