#pragma once

#include <cstdint>
#include <vector>

namespace rtype::network {

/**
 * @brief What an entity is, as the wire names it. The values are the ones of
 * the protocol page and never change.
 */
enum class WireEntityType : std::uint8_t {
  PlayerShip = 1,
  PlayerMissile = 2,
  Bydo = 3,
  EnemyMissile = 4,
};

/**
 * @brief One player of a game, as a WorldState lists it.
 */
struct PlayerState {
  std::uint8_t playerNumber;
  std::uint32_t shipEntityId;
  std::uint8_t lives;
  std::uint32_t score;
  bool invulnerable;

  bool operator==(const PlayerState&) const = default;
};

/**
 * @brief One entity of a game: its network id, what it is, and where its
 * center is, in logical units.
 */
struct EntityState {
  std::uint32_t entityId;
  WireEntityType entityType;
  float x;
  float y;

  bool operator==(const EntityState&) const = default;
};

/**
 * @brief The state of the game at one server tick.
 *
 * `tick` counts the simulation ticks of the server since the game started. A
 * state too big for one datagram is sent as several, with the same tick; each
 * holds every player and a part of the entities.
 */
struct WorldState {
  std::uint32_t tick;
  std::vector<PlayerState> players;
  std::vector<EntityState> entities;

  bool operator==(const WorldState&) const = default;
};

}  // namespace rtype::network
