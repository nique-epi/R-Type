#include "PlayerController.hpp"
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ShipInput.hpp"
#include "World.hpp"

namespace rtype::server {

void steerShipByInput(game::World& world, const ConnectedPlayer& player,
                      const network::PlayerInput& input) {
  auto* const intent = world.shipComponentOf<game::Intent>(player.shipEntityId);
  if (intent == nullptr) {
    return;
  }
  *intent = game::Intent{
      .directions = directionsOf(input),
      .shooting = holdsActionAt(input, network::SHOOT_ACTION_BIT_POSITION),
      .steered = true};
}

void silenceAwayShips(game::World& world, PlayerRoster& roster) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    if (!player->away) {
      continue;
    }
    auto* const intent =
        world.shipComponentOf<game::Intent>(player->shipEntityId);
    if (intent != nullptr) {
      *intent = game::Intent{};
    }
  }
}

}  // namespace rtype::server
