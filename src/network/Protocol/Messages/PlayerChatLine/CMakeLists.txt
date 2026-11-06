add_library(rtype_network_player_chat_line STATIC PlayerChatLine.cpp)

target_include_directories(rtype_network_player_chat_line PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_player_chat_line
  PUBLIC
    rtype_network_chat_line
    rtype_network_protocol
  PRIVATE
    rtype_network_byte_reader
    rtype_network_byte_writer
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_network_player_chat_line)
