add_subdirectory(ConnectionAccepted)
add_subdirectory(ConnectionChallenge)
add_subdirectory(ConnectionRefused)
add_subdirectory(ConnectionRequest)
add_subdirectory(Disconnection)
add_subdirectory(EntityDestroyed)
add_subdirectory(EntitySpawned)
add_subdirectory(GameEnded)
add_subdirectory(GameEvent)
add_subdirectory(GameStarted)
add_subdirectory(Heartbeat)
add_subdirectory(LobbyState)
add_subdirectory(PlayerInput)
add_subdirectory(PlayerLeft)
add_subdirectory(ReadyState)

add_library(rtype_network_messages INTERFACE)

target_link_libraries(rtype_network_messages
  INTERFACE
    rtype_network_connection_accepted
    rtype_network_connection_challenge
    rtype_network_connection_refused
    rtype_network_connection_request
    rtype_network_disconnection
    rtype_network_entity_destroyed
    rtype_network_entity_spawned
    rtype_network_game_ended
    rtype_network_game_event
    rtype_network_game_started
    rtype_network_heartbeat
    rtype_network_lobby_state
    rtype_network_player_input
    rtype_network_player_left
    rtype_network_ready_state
)
