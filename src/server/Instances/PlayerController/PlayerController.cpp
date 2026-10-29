#include "PlayerController.hpp"
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ShipInput.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @returns What the newest input of @p player wants: the directions and the
 * shot it holds, or nothing when the player sent no input yet.
 */
game::Intent intentOf(const ConnectedPlayer& player) {
  if (!player.newestInput.has_value()) {
    return game::Intent{};
  }
  return game::Intent{
      .directions = directionsOf(*player.newestInput),
      .shooting = holdsActionAt(*player.newestInput,
                                network::SHOOT_ACTION_BIT_POSITION)};
}

}  // namespace

void steerPlayerShips(game::World& world, PlayerRoster& roster) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    game::Intent* const intent = world.shipIntentOf(player->shipEntityId);
    if (intent != nullptr) {
      *intent = intentOf(*player);
    }
  }
}

}  // namespace rtype::server
