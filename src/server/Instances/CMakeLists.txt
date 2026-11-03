add_library(rtype_server_instances INTERFACE)

target_include_directories(rtype_server_instances INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances
  INTERFACE
    rtype_engine
    rtype_game
    rtype_game_bydo_kind
    rtype_game_waves
    rtype_network_transport
    rtype_server_constants
    rtype_server_player_roster
)

add_subdirectory(GameModules)
add_subdirectory(GameReplication)
add_subdirectory(ReplicationModule)
add_subdirectory(GameInstance)
