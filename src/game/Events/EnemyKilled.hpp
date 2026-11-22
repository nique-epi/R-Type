#pragma once

#include <array>
#include <cstdint>
#include "PilotCountConstants.hpp"

namespace rtype::game {

/**
 * @brief Event published when a missile of a player destroys an enemy that is
 * worth points.
 *
 * It names the player who fired and the points of the enemy, so whoever keeps
 * the score needs nothing else from the world. A Boss also names the damage
 * each pilot dealt to it, so its points are shared by that damage, and its
 * bounty.
 */
struct EnemyKilled {
  std::uint8_t playerNumber;
  std::uint32_t points;
  /** @brief The damage each pilot dealt to it, pilot FIRST_PILOT_NUMBER
   * first; all 0 for an enemy that keeps no tally, anything but a Boss. */
  std::array<std::uint32_t, MOST_PILOTS> damageByPilot{};
  /** @brief The kopecks every pilot present earns for it: the bounty of a
   * Boss, 0 for any other enemy. */
  std::uint32_t bounty{0};

  bool operator==(const EnemyKilled&) const = default;
};

}  // namespace rtype::game
