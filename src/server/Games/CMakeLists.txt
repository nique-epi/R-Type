add_library(rtype_server_games_module STATIC
  FarewellSender.cpp
  GamePlacement.cpp
  RunningGame.cpp
  PlayerDatagrams.cpp
  RtypeGameModules.cpp
  EntitySheetsFingerprint.cpp
  ServerGamesModule.cpp
  VanillaBreaches.cpp
)

target_include_directories(rtype_server_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_games_module
  PUBLIC
    rtype_engine_application
    rtype_game_events
    rtype_engine_event_bus
    rtype_engine_time
    rtype_game_entity_types
    rtype_logging
    rtype_server_exceptions
    rtype_server_constants
    rtype_server_instances
    rtype_server_replication_module
    rtype_server_results_module
    rtype_server_rules_module
    rtype_server_storage_module
    rtype_server_network_module
  PRIVATE
    rtype_network_disconnection
    rtype_network_handshake_cookies
    rtype_server_sessions_roster
    rtype_server_spawning
)

if(RTYPE_MODULE_CHAT)
  target_link_libraries(rtype_server_games_module PUBLIC rtype_server_chat_module)
endif()

rtype_enable_warnings(rtype_server_games_module)
