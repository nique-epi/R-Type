#include "WaveDirector.hpp"
#include <cstddef>
#include "BydoWave.hpp"
#include "GameException.hpp"
#include "TimeConstants.hpp"
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
                                  std::size_t bydosInPlay) {
  bydosDue_ = 0;
  const bool waveCleared = bydosLeftToAppear_ == 0 && bydosInPlay == 0;
  if (waveCleared) {
    startNextWave();
  }
  timers_.advance(elapsed);
  return bydosDue_;
}

std::size_t WaveDirector::waveNumber() const { return waveNumber_; }

BydoWave WaveDirector::currentWave() const { return wave_; }

void WaveDirector::startPause() {
  bydosLeftToAppear_ = wave_.bydoCount;
  timers_.scheduleOnce(curve_.wavePause, [this] { letABydoAppear(); });
}

void WaveDirector::startNextWave() {
  ++waveNumber_;
  wave_ = bydoWaveAfter(wave_, curve_);
  startPause();
}

void WaveDirector::letABydoAppear() {
  ++bydosDue_;
  --bydosLeftToAppear_;
  if (bydosLeftToAppear_ > 0) {
    timers_.scheduleOnce(wave_.spawnInterval, [this] { letABydoAppear(); });
  }
}

}  // namespace rtype::game
