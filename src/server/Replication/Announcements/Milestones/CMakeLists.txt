add_library(rtype_server_replication_announcements_milestones STATIC
  GameEndAnnouncer.cpp
  LobbyStateAnnouncer.cpp
  ShipSpawnAnnouncer.cpp
)

target_include_directories(rtype_server_replication_announcements_milestones PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_announcements_milestones
  PUBLIC
    rtype_game
    rtype_network_messages
    rtype_network_transport
    rtype_server_constants
    rtype_server_instances_context_setup
    rtype_server_replication_wire
    rtype_server_sessions_roster
  PRIVATE
    rtype_game_spawning
    rtype_identifiers
)

rtype_enable_warnings(rtype_server_replication_announcements_milestones)
