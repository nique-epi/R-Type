add_library(rtype_network_messages STATIC
  ConnectionAccepted.cpp
  ConnectionChallenge.cpp
  ConnectionRefused.cpp
  ConnectionRequest.cpp
  Disconnection.cpp
  EntityDestroyed.cpp
  EntitySpawned.cpp
  GameEnded.cpp
  GameEvent.cpp
  GameStarted.cpp
  Heartbeat.cpp
  LobbyState.cpp
  PlayerInput.cpp
  PlayerLeft.cpp
  ReadyState.cpp
)

target_include_directories(rtype_network_messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_messages
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_wire_types
    rtype_network_byte_writer
    rtype_network_byte_reader
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_messages)
