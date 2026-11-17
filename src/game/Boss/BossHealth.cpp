#include "BossHealth.hpp"
#include <algorithm>
#include <cstddef>
#include "BossConstants.hpp"
#include "BossHealthScaling.hpp"
#include "PercentConstants.hpp"

namespace rtype::game {

int scaledBossHealth(const BossHealthScaling& scaling) {
  const auto extraPlayers =
      static_cast<int>(std::max<std::size_t>(scaling.players, 1) - 1);
  const int forPlayers =
      scaling.points *
      (WHOLE_PERCENT + (BOSS_HEALTH_PER_EXTRA_PLAYER * extraPlayers)) /
      WHOLE_PERCENT;
  return std::max(1, forPlayers * scaling.healthPercent / WHOLE_PERCENT);
}

}  // namespace rtype::game
