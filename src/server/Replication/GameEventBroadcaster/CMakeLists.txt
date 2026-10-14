add_library(rtype_server_game_event_broadcaster STATIC GameEventBroadcaster.cpp)

target_include_directories(rtype_server_game_event_broadcaster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_event_broadcaster
  PUBLIC
    rtype_engine
    rtype_network_transport
    rtype_server_game_event_encoder
    rtype_server_replication_constants
  PRIVATE
    rtype_network_datagram_codec
)

rtype_enable_warnings(rtype_server_game_event_broadcaster)
