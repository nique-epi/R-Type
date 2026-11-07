#pragma once

#include <cstddef>
#include "PercentConstants.hpp"

namespace rtype::server {

/** @brief What the health of a boss is scaled from (see scaledBossHealth()):
 * the points its sheet gives, the players connected as it arrives, and the
 * health percent of the difficulty. */
struct BossHealthScaling {
  int sheetPoints{0};
  std::size_t players{0};
  int healthPercent{game::WHOLE_PERCENT};
};

}  // namespace rtype::server
