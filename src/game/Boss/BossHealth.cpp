#include "BossHealth.hpp"
#include <algorithm>
#include "BossConstants.hpp"
#include "BossHealthScaling.hpp"
#include "PercentConstants.hpp"
#include "TeamPercent.hpp"
#include "TeamScaling.hpp"
#include "ThreatPercent.hpp"
#include "ThreatScaling.hpp"

namespace rtype::game {

int scaledBossHealth(const BossHealthScaling& scaling) {
  const int forPlayers =
      scaling.points *
      teamPercent(
          TeamScaling{.percentPerExtraPlayer = BOSS_HEALTH_PER_EXTRA_PLAYER,
                      .players = scaling.players}) /
      WHOLE_PERCENT;
  const int forDifficulty = forPlayers * scaling.healthPercent / WHOLE_PERCENT;
  const int forThreat = forDifficulty *
                        threatenedPercent(ThreatScaling{
                            .percentAtTheMost = BOSS_HEALTH_AT_THE_MOST_THREAT,
                            .threatPercent = scaling.threatPercent}) /
                        WHOLE_PERCENT;
  return std::max(1, forThreat);
}

}  // namespace rtype::game
