#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The table `[gameplay.boss]` of the sheet of a boss: below which share
 * of its health it enrages, the attack it announces then, in the phase
 * Enraged, the attack of its arrival, during which no hit hurts it, and the
 * kopecks every pilot present earns when it is destroyed.
 */
struct BossSheet {
  /** @brief The percentage of its full health it enrages below, 1 to 99. */
  int rageBelowPercent{0};
  std::uint8_t rageAttack{0};
  std::uint8_t arrivalAttack{0};
  /** @brief 0 to the highest of BOUNTY_RANGE; 0 when the sheet leaves it
   * out. */
  std::uint32_t bounty{0};
};

}  // namespace rtype::game
