#include "WeaponKeeperFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "CrystalTouched.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Gun.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "ShippedEntitySheets.hpp"
#include "WeaponKeeper.hpp"
#include "WeaponKeeperParts.hpp"
#include "World.hpp"
#include "WorldEntityFixtures.hpp"

namespace rtype::server::testing {

WeaponKeeperFixture::WeaponKeeperFixture()
    : announcer_(ShipSpawnAnnouncerParts{
          .world = world_,
          .roster = roster_,
          .send =
              [this](network::OutgoingDatagram datagram) {
                sent_.push_back(std::move(datagram));
              },
          .ship = game::testing::shippedEntitySheets().named(
              game::testing::SHIP_SHEET)}),
      keeper_(std::make_unique<WeaponKeeper>(
          WeaponKeeperParts{.events = events_,
                            .world = world_,
                            .roster = roster_,
                            .types = game::testing::shippedEntitySheets()})) {
  events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& destruction) {
        destructions_.push_back(destruction);
      });
}

ConnectedPlayer& WeaponKeeperFixture::join(std::size_t index) {
  ConnectedPlayer* const player =
      roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN + index);
  announcer_.onPlayerJoined(*player);
  return *player;
}

std::uint32_t WeaponKeeperFixture::spawnCrystal(std::string_view sheet) {
  return world_.spawn(game::testing::shippedEntitySheets().named(sheet),
                      game::Position{});
}

void WeaponKeeperFixture::touch(const ConnectedPlayer& player,
                                std::uint32_t crystalId) {
  events_.publish(game::CrystalTouched{
      .ship = entityWithNetworkId(world_, player.shipEntityId),
      .crystal = entityWithNetworkId(world_, crystalId)});
  events_.dispatch();
}

void WeaponKeeperFixture::tick() {
  keeper_->onTick();
  events_.dispatch();
}

bool WeaponKeeperFixture::worldHolds(std::uint32_t networkId) {
  bool found = false;
  world_.components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity, const game::NetworkIdentity& identity,
                          const game::Position& /*position*/) {
        found = found || identity.networkId == networkId;
      });
  return found;
}

game::Gun WeaponKeeperFixture::gunOf(const ConnectedPlayer& player) {
  const game::Gun* const gun =
      world_.shipComponentOf<game::Gun>(player.shipEntityId);
  EXPECT_NE(gun, nullptr) << "the player has no ship";
  return gun == nullptr ? game::Gun{} : *gun;
}

std::size_t WeaponKeeperFixture::removalsOf(std::uint32_t networkId) const {
  return static_cast<std::size_t>(std::ranges::count(
      destructions_,
      game::EntityDestruction{.networkId = networkId,
                              .cause = game::DestructionCause::Removed}));
}

}  // namespace rtype::server::testing
