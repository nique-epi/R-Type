#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "ConnectedPlayerConstants.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "PlayerInput.hpp"

namespace rtype::server {

/**
 * @brief A player connected to the game: who the player is to the others, and
 * the session the server keeps with them.
 */
struct ConnectedPlayer {
  /** @brief FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; it also picks the color
   * of the player. */
  std::uint8_t playerNumber{};
  /** @brief 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8, as the request carried
   * it. */
  std::string nickname;
  /** @brief The address and port the player sends from, where every datagram
   * to the player goes. */
  network::Endpoint endpoint;
  /** @brief Drawn at random when the player was accepted, never 0; every
   * datagram of the session carries it. */
  std::uint64_t sessionToken{};
  /** @brief Sequence of the next datagram sent to the player: 0 for the first
   * one, then one more each time, wrapping from 65535 to 0. */
  std::uint16_t nextSequence{};
  /** @brief The network id of the ship of the player, NO_ENTITY_ID while it
   * has none. */
  std::uint32_t shipEntityId{identifiers::NO_ENTITY_ID};
  /** @brief The points the player earned in the current game; back to 0 when a
   * new world is built. */
  std::uint32_t score{0};
  /** @brief The lives the player has left, the ship in play included:
   * STARTING_LIVES when it joins and when a new world is built, one less each
   * time its ship is destroyed. At 0 the player has no ship and watches. */
  std::uint8_t lives{STARTING_LIVES};
  /** @brief Ticks of the game left before the ship of the player comes back
   * after it was destroyed: RESPAWN_DELAY_TICKS then, one less at each tick.
   * 0 while the player has a ship, or no life left. LifeKeeper counts it. */
  std::size_t ticksUntilRespawn{0};
  /** @brief Ticks of the game left during which the ship of the player cannot
   * be hit: INVULNERABILITY_TICKS when it comes back after a hit, one less at
   * each tick; 0 when it can be hit. LifeKeeper counts it. */
  std::size_t invulnerableTicksLeft{0};
  /** @brief Whether the player declared itself ready to start; cleared when the
   * game waits for players again. */
  bool ready{false};
  /** @brief The sequence of the newest datagram accepted from the player for
   * the messages that need an order, nothing before the first. */
  std::optional<std::uint16_t> newestReceivedSequence;
  /** @brief The PlayerInput with the highest inputNumber the player sent
   * during the session, to apply at every tick until a newer one arrives;
   * empty until the first one arrives. GameInstance keeps it. */
  std::optional<network::PlayerInput> newestInput;
  /** @brief Ticks the game ran since it last read a datagram of the player;
   * the session of the player closes once it reaches SESSION_TIMEOUT_TICKS. */
  std::size_t ticksSinceReceived{0};
  /** @brief Ticks that ended since the server last sent the player a datagram;
   * a Heartbeat goes out once it reaches TICKS_PER_HEARTBEAT.
   * nextSessionHeader() sets it back to 0. */
  std::size_t ticksSinceSent{0};
};

}  // namespace rtype::server
