add_library(rtype_client_entity_replicator STATIC EntityReplicator.cpp)

target_include_directories(rtype_client_entity_replicator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_replicator
  PUBLIC
    rtype_client_effect_spawner
    rtype_client_world_state_filter
    rtype_engine
    rtype_game_components
    rtype_network_entity_destroyed
    rtype_network_entity_spawned
    rtype_network_game_event
    rtype_network_protocol
  PRIVATE
    rtype_client_entity_sprites
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_entity_replicator)
