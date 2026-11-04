#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "MovementPattern.hpp"

namespace rtype::game {

/**
 * @brief The kinds of enemy; each one moves along its own pattern and is
 * drawn from its own sheet.
 *
 * Drifter and Weaver are the Bydos of the entity sheet. The other five are
 * declared for the pull request that gives each its behaviour: until then
 * they follow the pattern and earn the points of a Drifter.
 */
enum class EnemyKind : std::uint8_t {
  Drifter,
  Weaver,
  Voltjelly,
  Reaper,
  Bydomancer,
  Drillback,
  BydoTyrant,
};

/** @brief Every kind, in the order of the enumeration. */
constexpr std::array ALL_ENEMY_KINDS{
    EnemyKind::Drifter,   EnemyKind::Weaver,     EnemyKind::Voltjelly,
    EnemyKind::Reaper,    EnemyKind::Bydomancer, EnemyKind::Drillback,
    EnemyKind::BydoTyrant};

static_assert(ALL_ENEMY_KINDS.size() ==
                  static_cast<std::size_t>(EnemyKind::BydoTyrant) + 1,
              "ALL_ENEMY_KINDS lists every kind");

/**
 * @returns The pattern shape the enemies of @p kind follow: Weaver goes in a
 * wave, every other kind goes straight until its own pull request.
 */
[[nodiscard]] PatternShape patternShapeOf(EnemyKind kind);

/**
 * @returns The points the player who destroys an enemy of @p kind earns:
 * those of a Weaver for a Weaver, those of a Drifter for every other kind
 * until its own pull request.
 */
[[nodiscard]] std::uint32_t pointsOf(EnemyKind kind);

}  // namespace rtype::game
