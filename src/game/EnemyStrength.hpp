#pragma once

#include "PercentConstants.hpp"

namespace rtype::game {

/**
 * @brief How strong the enemies a world makes are, in percent of what their
 * sheets give: their health, rounded down and never under one point, and the
 * ticks of their gun, between two shots and before the first, rounded down
 * and never under one tick. A boss, which BossSystem scales on its own, and
 * any body that is not of side enemy keep their sheet.
 */
struct EnemyStrength {
  int healthPercent{WHOLE_PERCENT};
  int gunIntervalPercent{WHOLE_PERCENT};

  bool operator==(const EnemyStrength&) const = default;
};

}  // namespace rtype::game
