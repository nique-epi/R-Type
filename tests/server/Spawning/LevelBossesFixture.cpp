#include "LevelBossesFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include "Boss.hpp"
#include "BossArrival.hpp"
#include "BossEntranceParts.hpp"
#include "BossStageTestConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySheets.hpp"
#include "Health.hpp"
#include "LevelBosses.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "PercentConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

namespace rtype::server::testing {

LevelBossesFixture::LevelBossesFixture()
    : sheets_(game::EntitySheets::fromTexts(std::vector<game::SheetText>{
          game::testing::sheetText(WARLORD_NAME_IN_TESTS, WARLORD_SHEET_TEXT),
          game::testing::sheetText(STAGE_MINION_NAME_IN_TESTS,
                                   STAGE_MINION_SHEET_TEXT)})) {
  static_cast<void>(events_.subscribe<game::BossArrival>(
      [this](const game::BossArrival& arrival) {
        arrivals_.push_back(arrival);
      }));
  playDifficulty(game::WHOLE_PERCENT);
}

void LevelBossesFixture::playDifficulty(int percent) {
  bosses_ = std::make_unique<LevelBosses>(
      BossEntranceParts{.world = world_,
                        .roster = roster_,
                        .send = [](const network::OutgoingDatagram&) {},
                        .events = events_},
      sheets_, percent);
}

LevelBosses& LevelBossesFixture::bosses() { return *bosses_; }

void LevelBossesFixture::tick(std::size_t count) {
  for (std::size_t passed = 0; passed < count; ++passed) {
    bosses().onTick();
  }
}

std::vector<game::BossArrival> LevelBossesFixture::arrivals() {
  events_.dispatch();
  return arrivals_;
}

std::vector<int> LevelBossesFixture::bossHealths() {
  std::vector<std::pair<std::uint32_t, engine::Entity>> found;
  world_.components().forEach<game::NetworkIdentity, game::Boss>(
      [&found](engine::Entity entity, const game::NetworkIdentity& identity,
               const game::Boss&) {
        found.emplace_back(identity.networkId, entity);
      });
  std::ranges::sort(found, {},
                    [](const auto& bossFound) { return bossFound.first; });
  std::vector<int> healths;
  for (const auto& [networkId, entity] : found) {
    const game::Health* const health =
        world_.components().get<game::Health>(entity);
    healths.push_back(health == nullptr ? 0 : health->full);
  }
  return healths;
}

}  // namespace rtype::server::testing
