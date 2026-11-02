#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief The type byte of each message of the protocol page. The values never
 * change; a type byte outside this list is an unknown type.
 */
enum class MessageType : std::uint8_t {
  ConnectionRequest = 1,
  ConnectionAccepted = 2,
  ConnectionRefused = 3,
  Disconnection = 4,
  Heartbeat = 5,
  ReadyState = 6,
  LobbyState = 7,
  GameStarted = 8,
  PlayerInput = 9,
  WorldState = 10,
  EntitySpawned = 11,
  EntityDestroyed = 12,
  GameEvent = 13,
  PlayerLeft = 14,
  GameEnded = 15,
  ConnectionChallenge = 16,
  GameLoaded = 17,
};

}  // namespace rtype::network
