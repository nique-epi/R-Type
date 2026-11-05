#include "WavePlanFixtures.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BydoSpawnerTestConstants.hpp"
#include "WavePlan.hpp"
#include "game/Waves/WaveTestConstants.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The names of @p enemies, as a wave plan holds them. */
std::vector<std::string> namesOf(std::span<const std::string_view> enemies) {
  return {enemies.begin(), enemies.end()};
}

}  // namespace

WavePlan wavePlanOf(std::span<const std::string_view> enemies) {
  return WavePlan{.curve = game::testing::TEST_WAVE_CURVE,
                  .enemies = namesOf(enemies)};
}

WavePlan testWavePlan() { return wavePlanOf(TEST_ENEMY_CYCLE); }

WavePlan oneWaveLevelWavePlan() {
  return WavePlan{.curve = game::testing::ONE_WAVE_LEVEL_CURVE,
                  .enemies = namesOf(TEST_ENEMY_CYCLE)};
}

}  // namespace rtype::server::testing
