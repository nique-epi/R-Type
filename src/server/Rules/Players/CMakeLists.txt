add_library(rtype_server_rules_players STATIC
  LifeKeeper.cpp
  PlayerController.cpp
  PlayerLives.cpp
  ScoreKeeper.cpp
  ShipInput.cpp
  FavorTheShooter.cpp
  FavorThePlayers.cpp
  ShooterViews.cpp
)

target_include_directories(rtype_server_rules_players PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rules_players
  PUBLIC
    rtype_engine_event_bus
    rtype_game
    rtype_game_events
    rtype_game_lag_compensated_hit_system
    rtype_game_ship_control
    rtype_logging
    rtype_network_player_input
    rtype_server_instances_context
    rtype_server_sessions_roster
  PRIVATE
    rtype_identifiers
    rtype_network_protocol
)

rtype_enable_warnings(rtype_server_rules_players)
