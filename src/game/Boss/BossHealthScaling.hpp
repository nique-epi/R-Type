#pragma once

#include <cstddef>
#include "PercentConstants.hpp"

namespace rtype::game {

/** @brief What the health of a boss is scaled from (see scaledBossHealth()):
 * the points it appeared with, the players it is scaled for, and the health
 * percent of the difficulty. */
struct BossHealthScaling {
  int points{0};
  std::size_t players{0};
  int healthPercent{WHOLE_PERCENT};
};

}  // namespace rtype::game
