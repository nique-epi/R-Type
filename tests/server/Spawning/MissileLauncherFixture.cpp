#include "MissileLauncherFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "MissileOwner.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipStartPosition.hpp"

namespace rtype::server::testing {

MissileLauncherFixture::MissileLauncherFixture()
    : launcher_(world_, roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

ConnectedPlayer& MissileLauncherFixture::connect(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  player->shipEntityId = world_.spawnPlayer(
      game::shipStartPosition(player->playerNumber, network::MAXIMUM_PLAYERS));
  return *player;
}

void MissileLauncherFixture::runTicksShooting(ConnectedPlayer& shooter,
                                              std::size_t count) {
  ConnectedPlayers shooters;
  shooters.append(&shooter);
  for (std::size_t tick = 0; tick < count; ++tick) {
    launcher_.onTick(shooters);
  }
}

void MissileLauncherFixture::runTicksWithoutShooting(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    launcher_.onTick(ConnectedPlayers{});
  }
}

std::size_t MissileLauncherFixture::missilesInTheWorld() {
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

std::vector<std::uint8_t> MissileLauncherFixture::ownersOfTheMissiles() {
  std::vector<std::uint8_t> owners;
  world_.components().forEach<game::EntityType, game::MissileOwner>(
      [&owners](engine::Entity /*entity*/, const game::EntityType& /*type*/,
                const game::MissileOwner& owner) {
        owners.push_back(owner.playerNumber);
      });
  return owners;
}

std::vector<network::EntitySpawned> MissileLauncherFixture::spawnsSentTo(
    std::size_t index) const {
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

game::Position MissileLauncherFixture::shipOf(const ConnectedPlayer& player) {
  const game::Position* const ship = world_.shipPositionOf(player.shipEntityId);
  if (ship == nullptr) {
    ADD_FAILURE() << "the player has no ship";
    return game::Position{.x = 0.0F, .y = 0.0F};
  }
  return *ship;
}

}  // namespace rtype::server::testing
