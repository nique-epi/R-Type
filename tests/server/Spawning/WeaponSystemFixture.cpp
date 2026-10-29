#include "WeaponSystemFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BydoController.hpp"
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipStartPosition.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

WeaponSystemFixture::WeaponSystemFixture()
    : weapons_(world_, roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

ConnectedPlayer& WeaponSystemFixture::connect(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  player->shipEntityId = world_.spawnPlayer(
      game::shipStartPosition(player->playerNumber, network::MAXIMUM_PLAYERS));
  return *player;
}

void WeaponSystemFixture::connectWithoutShip(std::size_t index) {
  EXPECT_NE(roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                        SESSION_TOKEN),
            nullptr);
}

void WeaponSystemFixture::bydoAt(game::Position position) {
  world_.spawnBydo(position, game::BydoKind::Drifter);
}

void WeaponSystemFixture::runTicksShooting(ConnectedPlayer& shooter,
                                           std::size_t count) {
  releaseEveryone();
  setShooting(shooter, true);
  runTicks(count);
}

void WeaponSystemFixture::runTicksWithoutShooting(std::size_t count) {
  releaseEveryone();
  runTicks(count);
}

void WeaponSystemFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    steerBydos(world_);
    weapons_.onTick();
  }
}

void WeaponSystemFixture::setShooting(const ConnectedPlayer& player,
                                      bool shooting) {
  game::Intent* const intent = world_.shipIntentOf(player.shipEntityId);
  ASSERT_NE(intent, nullptr) << "the player has no ship";
  intent->shooting = shooting;
}

void WeaponSystemFixture::releaseEveryone() {
  for (const ConnectedPlayer* const player : roster_.connected()) {
    game::Intent* const intent = world_.shipIntentOf(player->shipEntityId);
    if (intent != nullptr) {
      intent->shooting = false;
    }
  }
}

std::size_t WeaponSystemFixture::missilesInTheWorld() {
  std::size_t missiles = 0;
  world_.components().forEach<game::EntityType, game::MissileOwner>(
      [&missiles](engine::Entity /*entity*/, const game::EntityType& type,
                  const game::MissileOwner& /*owner*/) {
        if (type == game::EntityType::PlayerMissile) {
          ++missiles;
        }
      });
  return missiles;
}

std::vector<std::uint8_t> WeaponSystemFixture::ownersOfTheMissiles() {
  std::vector<std::uint8_t> owners;
  world_.components().forEach<game::EntityType, game::MissileOwner>(
      [&owners](engine::Entity /*entity*/, const game::EntityType& /*type*/,
                const game::MissileOwner& owner) {
        owners.push_back(owner.playerNumber);
      });
  return owners;
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

game::Position WeaponSystemFixture::shipOf(const ConnectedPlayer& player) {
  const game::Position* const ship = world_.shipPositionOf(player.shipEntityId);
  if (ship == nullptr) {
    ADD_FAILURE() << "the player has no ship";
    return game::Position{.x = 0.0F, .y = 0.0F};
  }
  return *ship;
}

}  // namespace rtype::server::testing
