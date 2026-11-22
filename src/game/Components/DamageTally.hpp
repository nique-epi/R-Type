#pragma once

#include <array>
#include <cstdint>
#include "GameRuleConstants.hpp"

namespace rtype::game {

/**
 * @brief The damage each pilot's missiles took from a body that shares its
 * points by damage (a boss): DamageRules adds to it, and the EnemyKilled of
 * the body carries it.
 */
struct DamageTally {
  /** @brief Pilot identifiers::FIRST_PLAYER_NUMBER first. */
  std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS> byPilot{};
};

}  // namespace rtype::game
