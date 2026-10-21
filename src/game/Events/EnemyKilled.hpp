#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when a missile of a player destroys an enemy that is
 * worth points.
 *
 * It names the player who fired and the points of the enemy, so whoever keeps
 * the score needs nothing else from the world.
 */
struct EnemyKilled {
  std::uint8_t playerNumber;
  std::uint32_t points;

  bool operator==(const EnemyKilled&) const = default;
};

}  // namespace rtype::game
