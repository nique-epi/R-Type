add_library(rtype_server_game_event_encoder STATIC GameEventEncoder.cpp)

target_include_directories(rtype_server_game_event_encoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_event_encoder
  PUBLIC
    rtype_game_events
    rtype_network_messages
    rtype_server_replication_constants
  PRIVATE
    rtype_server_wire_entity_type_mapping
)

rtype_enable_warnings(rtype_server_game_event_encoder)
