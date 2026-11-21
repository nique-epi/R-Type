#include "BossHealth.hpp"
#include <algorithm>
#include "BossConstants.hpp"
#include "BossHealthScaling.hpp"
#include "PercentConstants.hpp"
#include "TeamPercent.hpp"
#include "TeamScaling.hpp"

namespace rtype::game {

int scaledBossHealth(const BossHealthScaling& scaling) {
  const int forPlayers =
      scaling.points *
      teamPercent(
          TeamScaling{.percentPerExtraPlayer = BOSS_HEALTH_PER_EXTRA_PLAYER,
                      .players = scaling.players}) /
      WHOLE_PERCENT;
  return std::max(1, forPlayers * scaling.healthPercent / WHOLE_PERCENT);
}

}  // namespace rtype::game
