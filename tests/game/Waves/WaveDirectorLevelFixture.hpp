#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "WaveDirector.hpp"

namespace rtype::game::testing {

/** @brief Ticks a test runs on a level: how many, and how many enemies the
 * world holds at each of them. */
struct LevelTicks {
  std::size_t count{};
  std::size_t enemiesInPlay{};
};

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
   * @brief Runs @p ticks of @p director, with the same number of enemies in
   * the world at each.
   * @returns How many enemies were due in them.
   */
  static std::size_t runTicks(WaveDirector& director, LevelTicks ticks);

 private:
  WaveDirector oneWaveLevel_;
  WaveDirector twoWaveLevel_;
};

}  // namespace rtype::game::testing
