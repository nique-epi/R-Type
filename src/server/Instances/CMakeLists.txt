add_library(rtype_server_instances INTERFACE)

target_include_directories(rtype_server_instances INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_instances
  INTERFACE
    rtype_engine
    rtype_game
    rtype_network_transport
    rtype_server_player_roster
    rtype_server_wave_plan
    rtype_server_world_slot
)

add_subdirectory(WorldSlot)
add_subdirectory(ScoreKeeper)
add_subdirectory(LifeKeeper)
add_subdirectory(ShipInput)
add_subdirectory(GameFlow)
add_subdirectory(GameInstance)
