add_library(rtype_server_rules_module STATIC
  GameFlow.cpp
  LifeKeeper.cpp
  PlayerController.cpp
  PlayerLives.cpp
  RtypeRulesModule.cpp
  RtypeSystems.cpp
  ScoreKeeper.cpp
  ShipInput.cpp
  StartBarrier.cpp
  WorldSlot.cpp
)

target_include_directories(rtype_server_rules_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rules_module
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_install_context
    rtype_engine_systems
    rtype_engine_time
    rtype_game
    rtype_game_events
    rtype_game_ship_control
    rtype_logging
    rtype_network_player_input
    rtype_network_transport
    rtype_server_instances
    rtype_server_player_roster
    rtype_server_sessions
    rtype_server_spawning
  PRIVATE
    rtype_identifiers
    rtype_network_game_started
    rtype_network_protocol
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_rules_module)
