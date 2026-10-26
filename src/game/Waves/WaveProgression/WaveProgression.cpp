#include "WaveProgression.hpp"
#include <algorithm>
#include "BydoWave.hpp"
#include "TimeConstants.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

BydoWave bydoWaveAfter(BydoWave previous, const WaveCurve& curve) {
  const engine::Duration shorterInterval =
      previous.spawnInterval - curve.spawnIntervalCutPerWave;
  return BydoWave{
      .bydoCount = previous.bydoCount + curve.bydosAddedPerWave,
      .spawnInterval = std::max(shorterInterval, curve.shortestSpawnInterval)};
}

bool isPlayable(const WaveCurve& curve) {
  const bool holdsBydos = curve.firstWave.bydoCount > 0;
  const bool leavesTimeBetweenBydos =
      curve.shortestSpawnInterval > engine::Duration::zero() &&
      curve.shortestSpawnInterval <= curve.firstWave.spawnInterval;
  const bool hasNoNegativeDuration =
      curve.spawnIntervalCutPerWave >= engine::Duration::zero() &&
      curve.wavePause >= engine::Duration::zero();
  return holdsBydos && leavesTimeBetweenBydos && hasNoNegativeDuration;
}

}  // namespace rtype::game
