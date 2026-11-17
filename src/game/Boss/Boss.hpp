#pragma once

#include <cstdint>
#include "BossProgress.hpp"

namespace rtype::game {

/**
 * @brief Component of a boss, given by `[gameplay.boss]`. A boss is a body
 * like any other: BossSystem alone reads this component, and moves the boss
 * from its warning to its fight and its rage (see BossSystem).
 */
struct Boss {
  /** @brief The percentage of its full health it enrages below. */
  int rageBelowPercent{0};
  std::uint8_t rageAttack{0};
  std::uint8_t arrivalAttack{0};
  /** @brief How far it is; moved on by BossSystem only. */
  BossProgress progress{BossProgress::Created};
};

}  // namespace rtype::game
