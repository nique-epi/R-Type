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
  LeaderboardRequest = 18,
  LeaderboardAnswer = 19,
  DeltaWorldState = 20,
  RoundTrips = 21,
  EnemyAction = 22,
  OrderedMessage = 23,
  ServerInformationRequest = 24,
  ServerInformation = 25,
  ConnectionRedirect = 26,
  GameStatus = 27,
  GameStatusAcknowledged = 28,
  MasterProbe = 29,
  ChatLine = 30,
  PlayerChatLine = 31,
  BossStatus = 32,
};

}  // namespace rtype::network
