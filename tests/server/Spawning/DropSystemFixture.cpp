#include "DropSystemFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "CrystalDropped.hpp"
#include "DropSystemParts.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EntityTypesTestConstants.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

DropSystemFixture::DropSystemFixture()
    : drops_(DropSystemParts{.events = events_,
                             .world = world_,
                             .roster = roster_,
                             .send =
                                 [this](network::OutgoingDatagram datagram) {
                                   sent_.push_back(std::move(datagram));
                                 },
                             .types = game::testing::shippedEntitySheets()}) {}

void DropSystemFixture::connect(std::size_t index) {
  EXPECT_NE(roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                        SESSION_TOKEN + index),
            nullptr);
}

void DropSystemFixture::dropRedAt(game::Position where) {
  events_.publish(game::CrystalDropped{.sheetNumber = redCrystalNumber(),
                                       .position = where});
  events_.dispatch();
}

void DropSystemFixture::tick() { drops_.onTick(); }

std::vector<network::EntitySpawned> DropSystemFixture::spawnsSentTo(
    std::size_t index) const {
  return onlyMessagesIn<network::EntitySpawned>(
      sentTo(sent_, playerEndpoint(index)));
}

std::size_t DropSystemFixture::crystalsInTheWorld() {
  std::size_t crystals = 0;
  world_.components().forEach<game::EntityType, game::NetworkIdentity>(
      [&crystals](engine::Entity /*entity*/, const game::EntityType& type,
                  const game::NetworkIdentity& /*identity*/) {
        if (type == game::EntityType::Crystal) {
          ++crystals;
        }
      });
  return crystals;
}

std::uint8_t DropSystemFixture::redCrystalNumber() {
  return game::testing::shippedEntitySheets()
      .named(game::testing::CRYSTAL_RED_SHEET)
      .number;
}

}  // namespace rtype::server::testing
