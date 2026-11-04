#include "WavePlanFixtures.hpp"
#include <span>
#include <vector>
#include "BydoSpawnerTestConstants.hpp"
#include "EnemyKind.hpp"
#include "WavePlan.hpp"
#include "game/Waves/WaveTestConstants.hpp"

namespace rtype::server::testing {

WavePlan wavePlanOf(std::span<const game::EnemyKind> enemies) {
  return WavePlan{
      .curve = game::testing::TEST_WAVE_CURVE,
      .enemies = std::vector<game::EnemyKind>(enemies.begin(), enemies.end())};
}

WavePlan testWavePlan() { return wavePlanOf(TEST_ENEMY_CYCLE); }

}  // namespace rtype::server::testing
