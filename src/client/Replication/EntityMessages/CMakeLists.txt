add_library(rtype_client_entity_messages STATIC
  EntityMessageQueue.cpp
  WorldStateAssembler.cpp
)

target_include_directories(rtype_client_entity_messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_messages
  PUBLIC
    rtype_client_session
    rtype_client_waiting_room
    rtype_engine_time
    rtype_logging
    rtype_network_entity_destroyed
    rtype_network_entity_spawned
    rtype_network_game_ended
    rtype_network_game_event
    rtype_network_lobby_state
    rtype_network_message_dispatcher
    rtype_network_player_left
    rtype_network_protocol
  PRIVATE
    rtype_network_world_state_codec
    rtype_network_world_state_delta
)

rtype_enable_warnings(rtype_client_entity_messages)
