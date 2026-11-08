add_library(rtype_client_connection_chat STATIC ChatChannel.cpp)

target_include_directories(rtype_client_connection_chat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_chat
  PUBLIC
    rtype_client_session
    rtype_client_waiting_room
    rtype_engine_time
    rtype_network_chat_line
    rtype_network_lobby_state
    rtype_network_message_dispatcher
    rtype_network_player_chat_line
  PRIVATE
    rtype_network_protocol
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_connection_chat)
