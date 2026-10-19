add_library(rtype_server_bydo_spawner STATIC BydoSpawner.cpp)

target_include_directories(rtype_server_bydo_spawner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_bydo_spawner
  PUBLIC
    rtype_engine_timer_scheduler
    rtype_game
    rtype_logging
    rtype_server_bydo_spawn_constants
    rtype_server_entity_spawn_sender
    rtype_server_player_roster
  PRIVATE
    rtype_game_spawning
)

rtype_enable_warnings(rtype_server_bydo_spawner)
