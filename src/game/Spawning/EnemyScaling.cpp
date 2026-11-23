#include "EnemyScaling.hpp"
#include <cstddef>
#include "EnemyStrength.hpp"
#include "PercentConstants.hpp"
#include "SpawningConstants.hpp"
#include "TeamPercent.hpp"
#include "TeamScaling.hpp"
#include "ThreatPercent.hpp"
#include "ThreatScaling.hpp"

namespace rtype::game {

EnemyStrength scaledEnemyStrength(EnemyStrength difficulty,
                                  std::size_t players) {
  const int healthForPlayers = teamPercent(
      TeamScaling{.percentPerExtraPlayer = ENEMY_HEALTH_PER_EXTRA_PLAYER,
                  .players = players});
  const int gunForPlayers = teamPercent(
      TeamScaling{.percentPerExtraPlayer = ENEMY_GUN_INTERVAL_PER_EXTRA_PLAYER,
                  .players = players});
  return EnemyStrength{.healthPercent = difficulty.healthPercent *
                                        healthForPlayers / WHOLE_PERCENT,
                       .gunIntervalPercent = difficulty.gunIntervalPercent *
                                             gunForPlayers / WHOLE_PERCENT};
}

EnemyStrength threatenedEnemyStrength(EnemyStrength strength,
                                      int threatPercent) {
  const int healthForThreat = threatenedPercent(
      ThreatScaling{.percentAtTheMost = ENEMY_HEALTH_AT_THE_MOST_THREAT,
                    .threatPercent = threatPercent});
  const int gunForThreat = threatenedPercent(
      ThreatScaling{.percentAtTheMost = ENEMY_GUN_INTERVAL_AT_THE_MOST_THREAT,
                    .threatPercent = threatPercent});
  return EnemyStrength{
      .healthPercent = strength.healthPercent * healthForThreat / WHOLE_PERCENT,
      .gunIntervalPercent =
          strength.gunIntervalPercent * gunForThreat / WHOLE_PERCENT};
}

}  // namespace rtype::game
