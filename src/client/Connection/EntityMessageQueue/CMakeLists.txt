add_library(rtype_client_entity_message_queue STATIC EntityMessageQueue.cpp)

target_include_directories(rtype_client_entity_message_queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_message_queue
  PUBLIC
    rtype_client_connection
    rtype_client_game_event_history
    rtype_client_waiting_room
    rtype_engine_time
    rtype_network_message_dispatcher
  PRIVATE
    rtype_network_world_state_codec
)

rtype_enable_warnings(rtype_client_entity_message_queue)
