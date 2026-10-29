#include "PlayerController.hpp"
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "PlayerRoster.hpp"
#include "ShipInput.hpp"
#include "World.hpp"

namespace rtype::server {

void steerPlayerShips(game::World& world, PlayerRoster& roster) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    game::Intent* const intent = world.shipIntentOf(player->shipEntityId);
    if (intent == nullptr) {
      continue;
    }
    intent->directions = player->newestInput.has_value()
                             ? directionsOf(*player->newestInput)
                             : game::ShipDirections{};
  }
}

}  // namespace rtype::server
