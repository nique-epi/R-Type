#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The table `[gameplay.boss]` of the sheet of a boss: below which share
 * of its health it enrages, the attack it announces then, in the phase
 * Enraged, and the attack of its arrival, during which no hit hurts it.
 */
struct BossSheet {
  /** @brief The percentage of its full health it enrages below, 1 to 99. */
  int rageBelowPercent{0};
  std::uint8_t rageAttack{0};
  std::uint8_t arrivalAttack{0};
};

}  // namespace rtype::game
