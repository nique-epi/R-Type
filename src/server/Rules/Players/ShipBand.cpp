#include "ShipBand.hpp"
#include "ConnectedPlayer.hpp"
#include "PlayBand.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

void bandEveryShip(game::World& world, PlayerRoster& roster,
                   game::PlayBand band) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    if (auto* const kept =
            world.shipComponentOf<game::PlayBand>(player->shipEntityId)) {
      *kept = band;
    }
  }
}

}  // namespace rtype::server
