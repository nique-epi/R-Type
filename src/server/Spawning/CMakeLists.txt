find_package(tomlplusplus CONFIG REQUIRED)

add_library(rtype_server_spawning STATIC BydoSpawner.cpp WaveCatalog.cpp WeaponSystem.cpp)

target_include_directories(rtype_server_spawning PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_spawning
  PUBLIC
    rtype_game
    rtype_game_components
    rtype_game_entity_types
    rtype_game_spawning
    rtype_game_wave_director
    rtype_game_waves
    rtype_logging
    rtype_server_exceptions
    rtype_server_instances_context
    rtype_server_replication_wire
    rtype_server_sessions_roster
  PRIVATE
    rtype_engine_data_files
    rtype_network_handshake
    tomlplusplus::tomlplusplus
)

rtype_enable_warnings(rtype_server_spawning)
