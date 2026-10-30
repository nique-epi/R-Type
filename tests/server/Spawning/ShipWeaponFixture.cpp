#include "ShipWeaponFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "MissileOwner.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipStartPosition.hpp"

namespace rtype::server::testing {

ConnectedPlayer& ShipWeaponFixture::connect(std::size_t index) {
  ConnectedPlayer* const player = roster().add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  player->shipEntityId = world().spawnPlayer(
      game::shipStartPosition(player->playerNumber, network::MAXIMUM_PLAYERS));
  return *player;
}

void ShipWeaponFixture::runTicksShooting(ConnectedPlayer& shooter,
                                         std::size_t count) {
  releaseEveryone();
  setShooting(shooter, true);
  runTicks(count);
}

void ShipWeaponFixture::runTicksWithoutShooting(std::size_t count) {
  releaseEveryone();
  runTicks(count);
}

void ShipWeaponFixture::setShooting(const ConnectedPlayer& player,
                                    bool shooting) {
  auto* const intent =
      world().shipComponentOf<game::Intent>(player.shipEntityId);
  ASSERT_NE(intent, nullptr) << "the player has no ship";
  intent->shooting = shooting;
}

void ShipWeaponFixture::releaseEveryone() {
  for (const ConnectedPlayer* const player : roster().connected()) {
    auto* const intent =
        world().shipComponentOf<game::Intent>(player->shipEntityId);
    if (intent != nullptr) {
      intent->shooting = false;
    }
  }
}

std::size_t ShipWeaponFixture::missilesInTheWorld() {
  std::size_t missiles = 0;
  world().components().forEach<game::EntityType, game::MissileOwner>(
      [&missiles](engine::Entity /*entity*/, const game::EntityType& type,
                  const game::MissileOwner& /*owner*/) {
        if (type == game::EntityType::PlayerMissile) {
          ++missiles;
        }
      });
  return missiles;
}

std::vector<std::uint8_t> ShipWeaponFixture::ownersOfTheMissiles() {
  std::vector<std::uint8_t> owners;
  world().components().forEach<game::EntityType, game::MissileOwner>(
      [&owners](engine::Entity /*entity*/, const game::EntityType& /*type*/,
                const game::MissileOwner& owner) {
        owners.push_back(owner.playerNumber);
      });
  return owners;
}

game::Position ShipWeaponFixture::shipOf(const ConnectedPlayer& player) {
  const game::Position* const ship =
      world().shipPositionOf(player.shipEntityId);
  if (ship == nullptr) {
    ADD_FAILURE() << "the player has no ship";
    return game::Position{.x = 0.0F, .y = 0.0F};
  }
  return *ship;
}

}  // namespace rtype::server::testing
