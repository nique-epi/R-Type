add_library(rtype_server_spawning STATIC WaveSpawner.cpp WaveCatalog.cpp DifficultyLayers.cpp WeaponSystem.cpp VolleySystem.cpp BossStage.cpp EntityOrders.cpp LevelDirector.cpp)

target_include_directories(rtype_server_spawning PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_spawning
  PUBLIC
    rtype_game
    rtype_game_components
    rtype_game_entity_types
    rtype_game_events
    rtype_game_level
    rtype_game_scripting
    rtype_game_spawning
    rtype_game_wave_director
    rtype_game_waves
    rtype_logging
    rtype_server_exceptions
    rtype_server_instances_context
    rtype_server_replication_wire
    rtype_server_sessions_roster
    rtype_server_sessions_tracking
  PRIVATE
    rtype_engine_data_files
    rtype_identifiers
    rtype_network_handshake
    rtype_network_protocol
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_server_spawning)
