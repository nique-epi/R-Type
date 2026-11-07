#include "VolleySystemFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ActionKey.hpp"
#include "EnemyCount.hpp"
#include "Entity.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumber.hpp"
#include "Intent.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "VolleySystemParts.hpp"
#include "VolleySystemTestConstants.hpp"

namespace rtype::server::testing {

VolleySystemFixture::VolleySystemFixture()
    : sheets_(game::EntitySheets::fromTexts(std::vector<game::SheetText>{
          game::testing::sheetText(BROODER_NAME_IN_TESTS, BROODER_SHEET_TEXT),
          game::testing::sheetText(MINION_NAME_IN_TESTS, MINION_SHEET_TEXT)})),
      volleys_(
          VolleySystemParts{.world = world_,
                            .roster = roster_,
                            .send =
                                [this](network::OutgoingDatagram datagram) {
                                  sent_.push_back(std::move(datagram));
                                },
                            .types = sheets_}) {}

void VolleySystemFixture::addBrooder() {
  world_.spawn(sheets_.named(BROODER_NAME_IN_TESTS), BROODER_CENTER);
}

void VolleySystemFixture::connectPlayers(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    EXPECT_NE(roster_.add(playerEndpoint(index),
                          std::string{PLAYER_NICKNAME} + std::to_string(index),
                          SESSION_TOKEN + index),
              nullptr);
  }
}

void VolleySystemFixture::startAction(game::ActionKey action) {
  const std::uint8_t brooder = sheets_.named(BROODER_NAME_IN_TESTS).number;
  world_.components().forEach<game::Intent, game::EntityTypeNumber>(
      [action, brooder](engine::Entity, game::Intent& intent,
                        const game::EntityTypeNumber& type) {
        if (type.number == brooder) {
          intent.startsAction = action;
        }
      });
}

void VolleySystemFixture::runTick() { volleys_.onTick(); }

std::vector<game::Position> VolleySystemFixture::minions() {
  std::vector<game::Position> found;
  const std::uint8_t minion = minionNumber();
  world_.components().forEach<game::Position, game::EntityTypeNumber>(
      [&found, minion](engine::Entity, const game::Position& position,
                       const game::EntityTypeNumber& type) {
        if (type.number == minion) {
          found.push_back(position);
        }
      });
  std::ranges::sort(found, {}, &game::Position::y);
  return found;
}

std::size_t VolleySystemFixture::enemies() {
  return game::enemiesIn(world_.components());
}

std::vector<network::EntitySpawned> VolleySystemFixture::spawnsSentTo(
    std::size_t index) const {
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

std::uint8_t VolleySystemFixture::minionNumber() const {
  return sheets_.named(MINION_NAME_IN_TESTS).number;
}

}  // namespace rtype::server::testing
