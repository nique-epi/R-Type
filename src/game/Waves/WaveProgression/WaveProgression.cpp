#include "WaveProgression.hpp"
#include <algorithm>
#include "TimeConstants.hpp"
#include "Wave.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

Wave waveAfter(Wave previous, const WaveCurve& curve) {
  const engine::Duration shorterInterval =
      previous.spawnInterval - curve.spawnIntervalCutPerWave;
  return Wave{
      .entityCount = previous.entityCount + curve.entitiesAddedPerWave,
      .spawnInterval = std::max(shorterInterval, curve.shortestSpawnInterval)};
}

bool isPlayable(const WaveCurve& curve) {
  const bool holdsEnemies = curve.firstWave.entityCount > 0;
  const bool leavesTimeBetweenEnemies =
      curve.shortestSpawnInterval > engine::Duration::zero() &&
      curve.shortestSpawnInterval <= curve.firstWave.spawnInterval;
  const bool hasNoNegativeDuration =
      curve.spawnIntervalCutPerWave >= engine::Duration::zero() &&
      curve.wavePause >= engine::Duration::zero();
  const bool holdsWaves = curve.levelWaves > 0;
  return holdsEnemies && leavesTimeBetweenEnemies && hasNoNegativeDuration &&
         holdsWaves;
}

}  // namespace rtype::game
