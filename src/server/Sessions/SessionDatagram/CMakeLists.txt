add_library(rtype_server_session_datagram STATIC SessionDatagram.cpp)

target_include_directories(rtype_server_session_datagram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_session_datagram
  PUBLIC
    rtype_network_protocol
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_server_session_datagram)
