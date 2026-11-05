#pragma once

#include <cstddef>
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "Wave.hpp"
#include "WaveConstants.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

/**
 * @brief Decides when the enemies of a game appear: in waves that grow along a
 * WaveCurve (see waveAfter), a pause before each.
 *
 * The first wave starts with a pause of the curve; then its enemies are due one
 * by one, the first at the end of the pause, the next ones a spawn interval of
 * the wave apart. Once every enemy of a wave is due, the next wave waits for a
 * step that finds no enemy in the world, then pauses, counted from the start of
 * that step. An enemy gone before the last of its wave is due does not end the
 * wave early. The level holds levelWaves waves of the curve: once the last
 * of them is over, no wave follows and lastWaveFinished() says so.
 *
 * The director creates nothing: advance() says how many enemies are due, its
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
   * @param enemiesInPlay How many enemies the world holds at the start of this
   * step, those created after the previous step included.
   * @returns How many enemies must appear now; 0 on most steps.
   * @throws NegativeElapsedTimeException when @p elapsed is negative.
   */
  [[nodiscard]] std::size_t advance(engine::Duration elapsed,
                                    std::size_t enemiesInPlay);

  /**
   * @returns The number of the wave whose enemies are coming or in play,
   * FIRST_WAVE_NUMBER at first. It moves on at the start of the pause before
   * the next wave.
   */
  [[nodiscard]] std::size_t waveNumber() const;

  /** @returns The enemies of that wave. */
  [[nodiscard]] Wave currentWave() const;

  /**
   * @returns Whether the last wave of the level is over: every one of its
   * enemies appeared and a step found none in the world. It stays true, and
   * no enemy is due any more. Whoever ends the level asks it, and may wait
   * for more than this, such as a boss, before it does.
   */
  [[nodiscard]] bool lastWaveFinished() const;

 private:
  void startPause();
  void startNextWave();
  void endTheWave();
  void letAnEnemyAppear();

  WaveCurve curve_;
  Wave wave_;
  std::size_t waveNumber_{FIRST_WAVE_NUMBER};
  std::size_t enemiesLeftToAppear_{0};
  std::size_t enemiesDue_{0};
  bool lastWaveFinished_{false};
  engine::TimerScheduler timers_;
};

}  // namespace rtype::game
