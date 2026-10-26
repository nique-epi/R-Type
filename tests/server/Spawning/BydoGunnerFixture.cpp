#include "BydoGunnerFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

BydoGunnerFixture::BydoGunnerFixture()
    : gunner_(world_, roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

void BydoGunnerFixture::connect(std::size_t index) {
  EXPECT_NE(roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                        SESSION_TOKEN),
            nullptr);
}

void BydoGunnerFixture::bydoAt(game::Position position) {
  world_.spawnBydo(position, game::BydoKind::Drifter);
}

void BydoGunnerFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    gunner_.onTick();
  }
}

std::vector<network::EntitySpawned>
BydoGunnerFixture::enemyMissilesInTheWorld() {
  std::vector<network::EntitySpawned> missiles;
  engine::ComponentRegistry& components = world_.components();
  components.forEach<game::EntityType, game::NetworkIdentity>(
      [&missiles, &components](engine::Entity entity,
                               const game::EntityType& type,
                               const game::NetworkIdentity& identity) {
        const game::Position* const position =
            components.get<game::Position>(entity);
        if (type != game::EntityType::EnemyMissile || position == nullptr) {
          return;
        }
        missiles.push_back(network::EntitySpawned{
            .entityId = identity.networkId,
            .entityType = network::WireEntityType::EnemyMissile,
            .x = position->x,
            .y = position->y});
      });
  return missiles;
}

std::vector<network::OutgoingDatagram> BydoGunnerFixture::datagramsSentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

}  // namespace rtype::server::testing
