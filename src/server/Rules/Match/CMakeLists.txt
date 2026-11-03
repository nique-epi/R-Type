add_library(rtype_server_rules_match STATIC
  GameFlow.cpp
  RtypeSystems.cpp
  StartBarrier.cpp
  WorldSlot.cpp
)

target_include_directories(rtype_server_rules_match PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rules_match
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_time
    rtype_game
    rtype_logging
    rtype_network_transport
    rtype_server_instances_context
    rtype_server_sessions_roster
    rtype_server_rules_players
    rtype_server_spawning
  PRIVATE
    rtype_identifiers
    rtype_network_game_started
)

rtype_enable_warnings(rtype_server_rules_match)
