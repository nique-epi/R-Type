#include "WeaponSystemFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "EntityTypesTestConstants.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "TimeConstants.hpp"
#include "WeaponSystemParts.hpp"

namespace rtype::server::testing {

WeaponSystemFixture::WeaponSystemFixture()
    : weapons_(
          WeaponSystemParts{.world = world_,
                            .roster = roster_,
                            .send =
                                [this](network::OutgoingDatagram datagram) {
                                  sent_.push_back(std::move(datagram));
                                },
                            .types = game::testing::shippedEntitySheets(),
                            .events = events_}),
      pilots_(events_) {}

void WeaponSystemFixture::connectWithoutShip(std::size_t index) {
  EXPECT_NE(roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                        SESSION_TOKEN),
            nullptr);
}

void WeaponSystemFixture::bydoAt(game::Position position) {
  world_.spawn(
      game::testing::shippedEntitySheets().named(game::testing::DRIFTER_SHEET),
      position);
}

void WeaponSystemFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    pilots_.update(world_.components(), engine::SIMULATION_TICK_DURATION);
    weapons_.onTick();
  }
}

std::vector<network::EntitySpawned>
WeaponSystemFixture::enemyMissilesInTheWorld() {
  std::vector<network::EntitySpawned> missiles;
  engine::ComponentRegistry& components = world_.components();
  components.forEach<game::EntityType, game::NetworkIdentity>(
      [&missiles, &components](engine::Entity entity,
                               const game::EntityType& type,
                               const game::NetworkIdentity& identity) {
        const game::Position* const position =
            components.get<game::Position>(entity);
        const game::EntityTypeNumber* const number =
            components.get<game::EntityTypeNumber>(entity);
        if (type != game::EntityType::EnemyMissile || position == nullptr ||
            number == nullptr) {
          return;
        }
        missiles.push_back(
            network::EntitySpawned{.entityId = identity.networkId,
                                   .entityType = number->number,
                                   .x = position->x,
                                   .y = position->y});
      });
  return missiles;
}

std::vector<network::EntitySpawned> WeaponSystemFixture::spawnsSentTo(
    std::size_t index) const {
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram : datagramsSentTo(index)) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

std::vector<network::OutgoingDatagram> WeaponSystemFixture::datagramsSentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

game::World& WeaponSystemFixture::world() { return world_; }

PlayerRoster& WeaponSystemFixture::roster() { return roster_; }

}  // namespace rtype::server::testing
