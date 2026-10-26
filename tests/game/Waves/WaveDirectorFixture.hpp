#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "WaveDirector.hpp"

namespace rtype::game::testing {

/**
 * @brief A wave director on TEST_WAVE_CURVE, run tick by tick, told by the
 * test how many Bydos the world holds.
 */
class WaveDirectorFixture : public ::testing::Test {
 protected:
  WaveDirectorFixture();

  /**
   * @brief Runs @p count ticks, with one Bydo in the world at each.
   * @returns How many Bydos were due in them.
   */
  std::size_t runTicksWithABydoInPlay(std::size_t count);

  /** @brief Runs one tick with no Bydo in the world. */
  void runATickWithTheSkyClear();

  /**
   * @brief Runs ticks, with @p bydosInPlay Bydos in the world at each, until a
   * Bydo is due, at most A_MINUTE_OF_TICKS.
   * @returns How many ticks ran, the one the Bydo is due on included; 0, after
   * a failure, when none was due.
   */
  std::size_t ticksToTheNextBydo(std::size_t bydosInPlay);

  /**
   * @brief Runs ticks until every Bydo of the first wave is due, one Bydo in
   * the world at each.
   */
  void letTheFirstWaveAppear();

  [[nodiscard]] const WaveDirector& director() const;

 private:
  WaveDirector director_;
};

}  // namespace rtype::game::testing
