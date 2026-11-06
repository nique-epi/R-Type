#include "ShardBurstFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BodyPlacement.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "Pilot.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShardBurstTestConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "WeaponSystemParts.hpp"

namespace rtype::server::testing {

namespace sheets = game::testing;

ShardBurstFixture::ShardBurstFixture()
    : sheets_(game::EntitySheets::fromTexts(std::vector<game::SheetText>{
          sheets::sheetText(sheets::ORB_NAME_IN_TESTS, sheets::ORB_SHEET_TEXT),
          sheets::sheetText(sheets::SHARD_NAME_IN_TESTS,
                            sheets::SHARD_SHEET_TEXT)})),
      weapons_(
          WeaponSystemParts{.world = world_,
                            .roster = roster_,
                            .send =
                                [this](network::OutgoingDatagram datagram) {
                                  sent_.push_back(std::move(datagram));
                                },
                            .types = sheets_,
                            .events = events_}) {
  events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& removal) {
        removals_.push_back(removal);
      });
}

std::uint32_t ShardBurstFixture::orbAt(game::Position center) {
  return world_.spawn(sheets_.named(sheets::ORB_NAME_IN_TESTS), center);
}

void ShardBurstFixture::connectPlayer() {
  EXPECT_NE(roster_.add(playerEndpoint(TOLD_PLAYER),
                        std::string{PLAYER_NICKNAME}, SESSION_TOKEN),
            nullptr);
}

void ShardBurstFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    weapons_.onTick();
  }
}

std::vector<game::BodyPlacement> ShardBurstFixture::shards() {
  std::vector<game::BodyPlacement> found;
  world_.components().forEach<game::Position, game::Pilot>(
      [&found](engine::Entity, const game::Position& position,
               const game::Pilot& pilot) {
        found.push_back(
            game::BodyPlacement{.position = position, .origin = pilot.origin});
      });
  return found;
}

std::vector<game::EntityDestruction> ShardBurstFixture::removals() {
  events_.dispatch();
  return removals_;
}

std::vector<network::EntitySpawned> ShardBurstFixture::spawnsSent() const {
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(TOLD_PLAYER))) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

}  // namespace rtype::server::testing
