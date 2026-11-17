#include "BossStageFixture.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Boss.hpp"
#include "BossStage.hpp"
#include "BossStageParts.hpp"
#include "BossStageTestConstants.hpp"
#include "Entity.hpp"
#include "EntitySheets.hpp"
#include "Position.hpp"
#include "RecordingEntityOrders.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

namespace rtype::server::testing {

BossStageFixture::BossStageFixture()
    : sheets_(game::EntitySheets::fromTexts(std::vector<game::SheetText>{
          game::testing::sheetText(WARLORD_NAME_IN_TESTS, WARLORD_SHEET_TEXT),
          game::testing::sheetText(STAGE_MINION_NAME_IN_TESTS,
                                   STAGE_MINION_SHEET_TEXT)})) {
  buildStage(std::string{WARLORD_NAME_IN_TESTS});
}

void BossStageFixture::buildStage(std::optional<std::string> boss) {
  stage_ = std::make_unique<BossStage>(BossStageParts{
      .world = world_, .orders = orders_, .boss = std::move(boss)});
}

void BossStageFixture::refuseTheBoss() {
  stage_ = std::make_unique<BossStage>(
      BossStageParts{.world = world_,
                     .orders = refusingOrders_,
                     .boss = std::string{WARLORD_NAME_IN_TESTS}});
}

void BossStageFixture::runTicks(std::size_t count, bool lastWaveFinished) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    stage_->onTick(lastWaveFinished);
  }
}

const std::vector<RecordedSpawn>& BossStageFixture::orders() const {
  return orders_.spawns();
}

void BossStageFixture::addBoss() {
  world_.spawn(sheets_.named(WARLORD_NAME_IN_TESTS), WHERE_THE_BOSS_IS_ORDERED);
}

void BossStageFixture::addMinion() {
  world_.spawn(sheets_.named(STAGE_MINION_NAME_IN_TESTS),
               game::Position{.x = 0.0F, .y = 0.0F});
}

void BossStageFixture::destroyTheBosses() {
  std::vector<engine::Entity> doomed;
  world_.components().forEach<game::Boss, game::Position>(
      [&doomed](engine::Entity boss, const game::Boss&, const game::Position&) {
        doomed.push_back(boss);
      });
  for (const engine::Entity boss : doomed) {
    world_.components().destroy(boss);
  }
}

bool BossStageFixture::cleared() const { return stage_->levelCleared(); }

}  // namespace rtype::server::testing
