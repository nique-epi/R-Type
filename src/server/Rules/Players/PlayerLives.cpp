#include "PlayerLives.hpp"
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "Lives.hpp"
#include "World.hpp"

namespace rtype::server {

std::uint8_t livesOf(game::World& world, const ConnectedPlayer& player) {
  const game::Lives* const lives =
      world.shipComponentOf<game::Lives>(player.shipEntityId);
  return lives != nullptr ? lives->count : player.livesWithoutShip;
}

void setLivesOf(game::World& world, ConnectedPlayer& player,
                std::uint8_t lives) {
  player.livesWithoutShip = lives;
  if (auto* const ship =
          world.shipComponentOf<game::Lives>(player.shipEntityId)) {
    ship->count = lives;
  }
}

}  // namespace rtype::server
