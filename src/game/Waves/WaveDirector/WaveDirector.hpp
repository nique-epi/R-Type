#pragma once

#include <cstddef>
#include "BydoWave.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "WaveConstants.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

/**
 * @brief Decides when the Bydos of a game appear: in waves that grow along a
 * WaveCurve (see bydoWaveAfter), a pause before each.
 *
 * The first wave starts with a pause of the curve; then its Bydos are due one
 * by one, the first at the end of the pause, the next ones a spawn interval of
 * the wave apart. Once every Bydo of a wave is due, the next wave waits for a
 * step that finds no Bydo in the world, then pauses, counted from the start of
 * that step. A Bydo gone before the last of its wave is due does not end the
 * wave early.
 *
 * The director creates nothing: advance() says how many Bydos are due, its
 * caller creates them and tells it, at the next step, how many the world
 * holds. Time is simulation time: with steps of SIMULATION_TICK_DURATION, a
 * little under a sixtieth of a second, a pause of 4 seconds ends on its 241st
 * tick.
 *
 * Neither copied nor moved: its timers call back into it.
 */
class WaveDirector {
 public:
  /**
   * @param curve How the waves grow.
   * @throws InvalidWaveCurveException when @p curve is not playable (see
   * isPlayable).
   */
  explicit WaveDirector(WaveCurve curve);
  WaveDirector(const WaveDirector&) = delete;
  WaveDirector& operator=(const WaveDirector&) = delete;
  WaveDirector(WaveDirector&&) = delete;
  WaveDirector& operator=(WaveDirector&&) = delete;
  ~WaveDirector() = default;

  /**
   * @brief Lets @p elapsed of simulation time pass.
   * @param bydosInPlay How many Bydos the world holds at the start of this
   * step, those created after the previous step included.
   * @returns How many Bydos must appear now; 0 on most steps.
   * @throws NegativeElapsedTimeException when @p elapsed is negative.
   */
  [[nodiscard]] std::size_t advance(engine::Duration elapsed,
                                    std::size_t bydosInPlay);

  /**
   * @returns The number of the wave whose Bydos are coming or in play,
   * FIRST_WAVE_NUMBER at first. It moves on at the start of the pause before
   * the next wave.
   */
  [[nodiscard]] std::size_t waveNumber() const;

  /** @returns The Bydos of that wave. */
  [[nodiscard]] BydoWave currentWave() const;

 private:
  void startPause();
  void startNextWave();
  void letABydoAppear();

  WaveCurve curve_;
  BydoWave wave_;
  std::size_t waveNumber_{FIRST_WAVE_NUMBER};
  std::size_t bydosLeftToAppear_{0};
  std::size_t bydosDue_{0};
  engine::TimerScheduler timers_;
};

}  // namespace rtype::game
