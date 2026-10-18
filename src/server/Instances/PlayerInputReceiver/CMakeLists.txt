add_library(rtype_server_player_input_receiver STATIC PlayerInputReceiver.cpp)

target_include_directories(rtype_server_player_input_receiver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_player_input_receiver
  PUBLIC
    rtype_network_message_dispatcher
    rtype_server_player_roster
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_server_player_input_receiver)
