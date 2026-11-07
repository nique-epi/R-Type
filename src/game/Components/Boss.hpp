#pragma once

#include <cstdint>

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

}  // namespace rtype::game
