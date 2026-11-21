#include "EnemyScaling.hpp"
#include <cstddef>
#include "EnemyStrength.hpp"
#include "PercentConstants.hpp"
#include "SpawningConstants.hpp"
#include "TeamPercent.hpp"
#include "TeamScaling.hpp"

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

}  // namespace rtype::game
