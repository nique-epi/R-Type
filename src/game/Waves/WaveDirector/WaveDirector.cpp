#include "WaveDirector.hpp"
#include <cstddef>
#include "GameException.hpp"
#include "TimeConstants.hpp"
#include "Wave.hpp"
#include "WaveCurve.hpp"
#include "WaveProgression.hpp"

namespace rtype::game {

WaveDirector::WaveDirector(WaveCurve curve)
    : curve_(curve), wave_(curve.firstWave) {
  if (!isPlayable(curve_)) {
    throw InvalidWaveCurveException();
  }
  startPause();
}

std::size_t WaveDirector::advance(engine::Duration elapsed,
                                  std::size_t enemiesInPlay) {
  enemiesDue_ = 0;
  const bool waveCleared = enemiesLeftToAppear_ == 0 && enemiesInPlay == 0;
  if (waveCleared) {
    endTheWave();
  }
  timers_.advance(elapsed);
  return enemiesDue_;
}

std::size_t WaveDirector::waveNumber() const { return waveNumber_; }

Wave WaveDirector::currentWave() const { return wave_; }

bool WaveDirector::lastWaveFinished() const { return lastWaveFinished_; }

void WaveDirector::startPause() {
  enemiesLeftToAppear_ = wave_.entityCount;
  timers_.scheduleOnce(curve_.wavePause, [this] { letAnEnemyAppear(); });
}

void WaveDirector::endTheWave() {
  if (waveNumber_ >= curve_.levelWaves) {
    lastWaveFinished_ = true;
    return;
  }
  startNextWave();
}

void WaveDirector::startNextWave() {
  ++waveNumber_;
  wave_ = waveAfter(wave_, curve_);
  startPause();
}

void WaveDirector::letAnEnemyAppear() {
  ++enemiesDue_;
  --enemiesLeftToAppear_;
  if (enemiesLeftToAppear_ > 0) {
    timers_.scheduleOnce(wave_.spawnInterval, [this] { letAnEnemyAppear(); });
  }
}

}  // namespace rtype::game
