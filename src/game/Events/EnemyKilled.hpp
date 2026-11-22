#pragma once

#include <array>
#include <cstdint>
#include "GameRuleConstants.hpp"

namespace rtype::game {

/**
 * @brief Event published when a missile of a player destroys an enemy that is
 * worth points.
 *
 * It names the player who fired and the points of the enemy, so whoever keeps
 * the score needs nothing else from the world. A body with a DamageTally (a
 * boss) also names the damage each pilot dealt to it, so its points are
 * shared by that damage, and the bounty of its ScoreValue.
 */
struct EnemyKilled {
  std::uint8_t playerNumber;
  std::uint32_t points;
  /** @brief The damage each pilot dealt to it (DamageTally); all 0 for an
   * enemy that keeps no tally. */
  std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS> damageByPilot{};
  /** @brief The kopecks every pilot present earns for it (ScoreValue). */
  std::uint32_t bounty{0};

  bool operator==(const EnemyKilled&) const = default;
};

}  // namespace rtype::game
