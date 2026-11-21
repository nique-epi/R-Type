#include "EnemyScaling.hpp"
#include <algorithm>
#include <cstddef>
#include "EnemyStrength.hpp"
#include "PercentConstants.hpp"
#include "SpawningConstants.hpp"

namespace rtype::game {

EnemyStrength scaledEnemyStrength(EnemyStrength difficulty,
                                  std::size_t players) {
  const auto extraPlayers =
      static_cast<int>(std::max<std::size_t>(players, 1) - 1);
  const int forPlayers =
      WHOLE_PERCENT + (ENEMY_HEALTH_PER_EXTRA_PLAYER * extraPlayers);
  return EnemyStrength{
      .healthPercent = difficulty.healthPercent * forPlayers / WHOLE_PERCENT,
      .gunIntervalPercent = difficulty.gunIntervalPercent};
}

}  // namespace rtype::game
