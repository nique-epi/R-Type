#include "LookItemKeeperFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityTypesTestConstants.hpp"
#include "LookItemKeeper.hpp"
#include "LookItemKeeperParts.hpp"
#include "LookItemTouched.hpp"
#include "LookItemWon.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The entity of @p world that carries @p networkId; a default one,
 * after a failure, when none does. */
engine::Entity entityOf(game::World& world, std::uint32_t networkId) {
  std::optional<engine::Entity> found;
  world.components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity entity,
                          const game::NetworkIdentity& identity,
                          const game::Position& /*position*/) {
        if (identity.networkId == networkId) {
          found = entity;
        }
      });
  EXPECT_TRUE(found.has_value());
  return found.value_or(engine::Entity{});
}

}  // namespace

LookItemKeeperFixture::LookItemKeeperFixture()
    : announcer_(ShipSpawnAnnouncerParts{
          .world = world_,
          .roster = roster_,
          .send =
              [this](network::OutgoingDatagram datagram) {
                sent_.push_back(std::move(datagram));
              },
          .ship = game::testing::shippedEntitySheets().named(
              game::testing::SHIP_SHEET)}),
      keeper_(std::make_unique<LookItemKeeper>(LookItemKeeperParts{
          .events = events_,
          .world = world_,
          .roster = roster_,
          .send = [this](network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          }})) {
  events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& destruction) {
        destructions_.push_back(destruction);
      });
}

ConnectedPlayer& LookItemKeeperFixture::join(std::size_t index) {
  ConnectedPlayer* const player =
      roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN + index);
  announcer_.onPlayerJoined(*player);
  return *player;
}

std::uint32_t LookItemKeeperFixture::spawnACrown() {
  return world_.spawn(game::testing::shippedEntitySheets().named(
                          game::testing::LOOK_CROWN_SHEET),
                      game::Position{});
}

void LookItemKeeperFixture::publishTouch(const ConnectedPlayer& player,
                                         std::uint32_t itemId) {
  events_.publish(
      game::LookItemTouched{.ship = entityOf(world_, player.shipEntityId),
                            .item = entityOf(world_, itemId)});
}

void LookItemKeeperFixture::deliver() { events_.dispatch(); }

void LookItemKeeperFixture::tick() { keeper_->onTick(); }

bool LookItemKeeperFixture::worldHolds(std::uint32_t networkId) {
  bool found = false;
  world_.components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity, const game::NetworkIdentity& identity,
                          const game::Position& /*position*/) {
        found = found || identity.networkId == networkId;
      });
  return found;
}

std::vector<network::LookItemWon> LookItemKeeperFixture::wonBy(
    std::size_t index) const {
  std::vector<network::LookItemWon> won;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    for (const network::LookItemWon& message :
         messagesIn<network::LookItemWon>(datagram)) {
      won.push_back(message);
    }
  }
  return won;
}

std::size_t LookItemKeeperFixture::removalsOf(std::uint32_t networkId) const {
  return static_cast<std::size_t>(std::ranges::count(
      destructions_,
      game::EntityDestruction{.networkId = networkId,
                              .cause = game::DestructionCause::Removed}));
}

}  // namespace rtype::server::testing
