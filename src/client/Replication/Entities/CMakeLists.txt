add_library(rtype_client_entities STATIC
  EnemyActionPlayer.cpp
  EntityCues.cpp
  EntityReplicator.cpp
  EntityTypeLooks.cpp
  LastListedTicks.cpp
  PositionHistory.cpp
  WorldStateFilter.cpp
)

target_include_directories(rtype_client_entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entities
  PUBLIC
    rtype_client_audio_contract_sounds
    rtype_client_connection_entity_types
    rtype_client_effect_spawner
    rtype_client_entity_appearances
    rtype_engine
    rtype_engine_time
    rtype_game_components
    rtype_network_enemy_action
    rtype_network_entity_destroyed
    rtype_network_entity_spawned
    rtype_network_game_event
    rtype_network_protocol
  PRIVATE
    rtype_client_entity_sprites
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_entities)
