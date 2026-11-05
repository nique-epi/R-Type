#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "WaveDirector.hpp"

namespace rtype::game::testing {

/**
 * @brief Two wave directors on the test curve, one whose level holds a single
 * wave and one whose level holds two, run tick by tick, told by the test how
 * many Bydos the world holds.
 */
class WaveDirectorLevelFixture : public ::testing::Test {
 protected:
  WaveDirectorLevelFixture();

  /** @returns The director whose level holds one wave. */
  [[nodiscard]] WaveDirector& oneWaveLevel();

  /** @returns The director whose level holds two waves. */
  [[nodiscard]] WaveDirector& twoWaveLevel();

  /**
   * @brief Runs @p count ticks of @p director, with @p bydosInPlay Bydos in
   * the world at each.
   * @returns How many Bydos were due in them.
   */
  static std::size_t runTicks(WaveDirector& director, std::size_t count,
                              std::size_t bydosInPlay);

 private:
  WaveDirector oneWaveLevel_;
  WaveDirector twoWaveLevel_;
};

}  // namespace rtype::game::testing
