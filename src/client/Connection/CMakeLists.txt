add_library(rtype_client_connection INTERFACE)

target_include_directories(rtype_client_connection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection
  INTERFACE
    rtype_client_action_set
    rtype_engine_time
    rtype_network_connection_refused
    rtype_network_entity_destroyed
    rtype_network_entity_spawned
    rtype_network_game_ended
    rtype_network_game_event
    rtype_network_lobby_state
    rtype_network_protocol
    rtype_network_transport
)

add_subdirectory(NetworkThread)
add_subdirectory(PlayerInputPacer)
add_subdirectory(GameEventHistory)
add_subdirectory(SessionOutbox)
add_subdirectory(SessionDeparture)
add_subdirectory(SessionInbound)
add_subdirectory(ConnectionAttempt)
add_subdirectory(SessionObservers)
add_subdirectory(ServerSession)
add_subdirectory(WaitingRoom)
add_subdirectory(EntityMessageQueue)
add_subdirectory(DatagramRoute)
add_subdirectory(DatagramRouter)
