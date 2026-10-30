add_library(rtype_server_instances INTERFACE)

target_include_directories(rtype_server_instances INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances
  INTERFACE
    rtype_engine
    rtype_game
    rtype_network_transport
    rtype_server_constants
    rtype_server_player_roster
    rtype_server_wave_plan
)

add_subdirectory(WorldSlot)
add_subdirectory(ScoreKeeper)
add_subdirectory(LifeKeeper)
add_subdirectory(ShipInput)
add_subdirectory(PlayerController)
add_subdirectory(PlayerLives)
add_subdirectory(GameFlow)
add_subdirectory(GameModules)
add_subdirectory(GameReplication)
add_subdirectory(ReplicationModule)
add_subdirectory(RtypeSystems)
add_subdirectory(RtypeRulesModule)
add_subdirectory(GameInstance)
