add_library(rtype_client_waiting_room STATIC
  GameLoadedReminder.cpp
  WaitingRoom.cpp
)

target_include_directories(rtype_client_waiting_room PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_waiting_room
  PUBLIC
    rtype_client_session
    rtype_engine_time
    rtype_logging
    rtype_network_lobby_state
    rtype_network_message_dispatcher
  PRIVATE
    rtype_network_game_loaded
    rtype_network_game_started
    rtype_network_protocol
    rtype_network_ready_state
)

rtype_enable_warnings(rtype_client_waiting_room)
