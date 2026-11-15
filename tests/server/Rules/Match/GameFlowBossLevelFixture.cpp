#include "GameFlowBossLevelFixture.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "Boss.hpp"
#include "BossProgress.hpp"
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
  return bossProgress().size();
}

std::vector<game::BossProgress> GameFlowBossLevelFixture::bossProgress() {
  std::vector<game::BossProgress> progress;
  flow().world().components().forEach<game::Boss, game::Position>(
      [&progress](engine::Entity, const game::Boss& boss,
                  const game::Position&) {
        progress.push_back(boss.progress);
      });
  return progress;
}

}  // namespace rtype::server::testing
