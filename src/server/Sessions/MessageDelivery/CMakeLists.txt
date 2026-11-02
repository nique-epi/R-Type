add_library(rtype_server_message_delivery STATIC MessageDelivery.cpp)

target_include_directories(rtype_server_message_delivery PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_message_delivery
  PUBLIC
    rtype_engine_time
    rtype_network_protocol
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_network_datagram_codec
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_message_delivery)
