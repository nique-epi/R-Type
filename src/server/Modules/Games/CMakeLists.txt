add_library(rtype_server_games_module STATIC
  FarewellSender.cpp
  Games.cpp
  PlayerDatagrams.cpp
  RtypeGameModules.cpp
  ServerGamesModule.cpp
)

target_include_directories(rtype_server_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_games_module
  PUBLIC
    rtype_engine_application
    rtype_engine_event_bus
    rtype_engine_time
    rtype_engine_timer_scheduler
    rtype_logging
    rtype_server_exceptions
    rtype_server_constants
    rtype_server_instances
    rtype_server_replication_module
    rtype_server_rules_module
    rtype_server_network_module
  PRIVATE
    rtype_network_disconnection
    rtype_server_game_placement
    rtype_server_module_constants
    rtype_server_session_datagram
    rtype_server_spawning
)

rtype_enable_warnings(rtype_server_games_module)
