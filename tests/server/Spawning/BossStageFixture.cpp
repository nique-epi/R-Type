#include "BossStageFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Boss.hpp"
#include "BossArrival.hpp"
#include "BossStageParts.hpp"
#include "BossStageTestConstants.hpp"
#include "Entity.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

namespace rtype::server::testing {

BossStageFixture::BossStageFixture()
    : sheets_(game::EntitySheets::fromTexts(std::vector<game::SheetText>{
          game::testing::sheetText(WARLORD_NAME_IN_TESTS, WARLORD_SHEET_TEXT),
          game::testing::sheetText(STAGE_MINION_NAME_IN_TESTS,
                                   STAGE_MINION_SHEET_TEXT)})) {
  events_.subscribe<game::BossArrival>(
      [this](const game::BossArrival& arrival) {
        arrivals_.push_back(arrival);
      });
  buildStage(std::string{WARLORD_NAME_IN_TESTS});
}

void BossStageFixture::buildStage(std::optional<std::string> boss) {
  stage_ = std::make_unique<BossStage>(
      BossStageParts{.world = world_,
                     .roster = roster_,
                     .send =
                         [this](network::OutgoingDatagram datagram) {
                           sent_.push_back(std::move(datagram));
                         },
                     .events = events_,
                     .types = sheets_,
                     .boss = std::move(boss)});
}

void BossStageFixture::runTicks(std::size_t count, bool lastWaveFinished) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    stage_->onTick(lastWaveFinished);
  }
}

void BossStageFixture::connectPlayers(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    EXPECT_NE(roster_.add(playerEndpoint(index),
                          std::string{PLAYER_NICKNAME} + std::to_string(index),
                          SESSION_TOKEN + index),
              nullptr);
  }
}

std::vector<game::BossArrival> BossStageFixture::arrivals() {
  events_.dispatch();
  return arrivals_;
}

std::vector<game::Position> BossStageFixture::bosses() {
  std::vector<game::Position> found;
  world_.components().forEach<game::Boss, game::Position>(
      [&found](engine::Entity, const game::Boss&,
               const game::Position& position) { found.push_back(position); });
  return found;
}

std::vector<network::EntitySpawned> BossStageFixture::spawnsSentTo(
    std::size_t index) const {
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
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

std::uint8_t BossStageFixture::warlordNumber() const {
  return sheets_.named(WARLORD_NAME_IN_TESTS).number;
}

}  // namespace rtype::server::testing
