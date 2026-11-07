#include "GameFlowBossLevelFixture.hpp"
#include <cstddef>
#include <string>
#include "Boss.hpp"
#include "Entity.hpp"
#include "GameFlowFixture.hpp"
#include "Position.hpp"
#include "Spawning/BossStageTestConstants.hpp"
#include "Spawning/WaveCatalogBossFixtures.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "WavePlan.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The level of one wave of the tests, ended by the warlord. */
WavePlan oneWaveLevelWithTheWarlord() {
  WavePlan waves = oneWaveLevelWavePlan();
  waves.boss = std::string{WARLORD_NAME_IN_TESTS};
  return waves;
}

}  // namespace

GameFlowBossLevelFixture::GameFlowBossLevelFixture()
    : GameFlowFixture(oneWaveLevelWithTheWarlord(), sheetsWithAWarlord()) {}

std::size_t GameFlowBossLevelFixture::bossesInPlay() {
  std::size_t bosses = 0;
  flow().world().components().forEach<game::Boss, game::Position>(
      [&bosses](engine::Entity, const game::Boss&, const game::Position&) {
        ++bosses;
      });
  return bosses;
}

}  // namespace rtype::server::testing
