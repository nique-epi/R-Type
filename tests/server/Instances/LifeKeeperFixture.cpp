#include "LifeKeeperFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BydoKind.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipContact.hpp"
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

LifeKeeperFixture::LifeKeeperFixture()
    : announcer_(world_, roster_,
                 [this](network::OutgoingDatagram datagram) {
                   sent_.push_back(std::move(datagram));
                 }),
      keeper_(std::make_unique<LifeKeeper>(
          LifeKeeperParts{.events = events_,
                          .world = world_,
                          .roster = roster_,
                          .respawn = [this](ConnectedPlayer& player) {
                            announcer_.onPlayerJoined(player);
                          }})) {
  events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& destruction) {
        destructions_.push_back(destruction);
      });
  events_.subscribe<game::PlayerHit>(
      [this](const game::PlayerHit& hit) { hits_.push_back(hit); });
  events_.subscribe<game::PlayerElimination>(
      [this](const game::PlayerElimination& elimination) {
        eliminations_.push_back(elimination);
      });
}

ConnectedPlayer& LifeKeeperFixture::join(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  announcer_.onPlayerJoined(*player);
  return *player;
}

std::uint32_t LifeKeeperFixture::bydoOn(const ConnectedPlayer& player) {
  const game::Position* const ship = world_.shipPositionOf(player.shipEntityId);
  EXPECT_NE(ship, nullptr);
  const game::Position position = ship == nullptr ? game::Position{} : *ship;
  return world_.spawnBydo(position, game::BydoKind::Drifter);
}

void LifeKeeperFixture::publishContact(std::uint32_t shipId,
                                       std::uint32_t enemyId) {
  events_.publish(game::ShipContact{.player = entityOf(world_, shipId),
                                    .enemy = entityOf(world_, enemyId)});
}

void LifeKeeperFixture::deliver() { events_.dispatch(); }

std::uint32_t LifeKeeperFixture::touchABydo(const ConnectedPlayer& player) {
  const std::uint32_t bydo = bydoOn(player);
  publishContact(player.shipEntityId, bydo);
  deliver();
  return bydo;
}

void LifeKeeperFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    keeper_->onTick();
  }
}

void LifeKeeperFixture::destroyTheKeeper() { keeper_.reset(); }

std::size_t LifeKeeperFixture::explosionsOf(std::uint32_t networkId) const {
  return static_cast<std::size_t>(std::ranges::count(
      destructions_,
      game::EntityDestruction{.networkId = networkId,
                              .cause = game::DestructionCause::Killed}));
}

const std::vector<game::PlayerHit>& LifeKeeperFixture::hits() const {
  return hits_;
}

const std::vector<game::PlayerElimination>& LifeKeeperFixture::eliminations()
    const {
  return eliminations_;
}

}  // namespace rtype::server::testing
