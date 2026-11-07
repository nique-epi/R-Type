#pragma once

#include <cstdint>
#include "PercentConstants.hpp"

namespace rtype::game {

/**
 * @brief Component of a boss, given by `[gameplay.boss]`.
 *
 * DamageRules takes nothing from it while it is in any phase of its arrival,
 * and announces its health after every hit; BossPhaseSystem announces its
 * fight once its arrival is over, and enrages it, once, when its health falls
 * below rageBelowPoints; PlayfieldExitSystem never removes it; the boss stage
 * of the server ends the level once it is gone.
 */
struct Boss {
  /** @brief Its full health times the percentage of its sheet, rounded down. */
  int rageBelowPoints{0};
  std::uint8_t rageAttack{0};
  std::uint8_t arrivalAttack{0};
  /** @brief Whether its fight was announced; set once by BossPhaseSystem. */
  bool fighting{false};
  /** @brief Whether it enraged; set once by BossPhaseSystem, never cleared. */
  bool enraged{false};
};

/** @returns The points a boss of @p fullHealth enrages below, for the
 * `rage_below_percent` @p rageBelowPercent of its sheet, rounded down. */
[[nodiscard]] constexpr int rageBelowPointsOf(int fullHealth,
                                              int rageBelowPercent) {
  return fullHealth * rageBelowPercent / WHOLE_PERCENT;
}

}  // namespace rtype::game
