add_library(rtype_server_entity_spawn_sender STATIC EntitySpawnSender.cpp)

target_include_directories(rtype_server_entity_spawn_sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_entity_spawn_sender
  PUBLIC
    rtype_network_datagram_codec
    rtype_network_entity_spawned
    rtype_network_transport
    rtype_server_player_roster
)

rtype_enable_warnings(rtype_server_entity_spawn_sender)
