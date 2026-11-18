#include "TransitionHold.hpp"
#include <algorithm>
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ShipMotion.hpp"
#include "ShipStartPosition.hpp"
#include "World.hpp"

namespace rtype::server {

void holdEveryShip(game::World& world, PlayerRoster& roster,
                   std::size_t ticksLeft) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    if (auto* const intent =
            world.shipComponentOf<game::Intent>(player->shipEntityId)) {
      *intent = game::Intent{};
    }
    if (auto* const motion =
            world.shipComponentOf<game::ShipMotion>(player->shipEntityId)) {
      *motion = game::ShipMotion{};
    }
    if (auto* const protection =
            world.shipComponentOf<game::Invulnerable>(player->shipEntityId)) {
      protection->ticksLeft = std::max(protection->ticksLeft, ticksLeft);
    }
  }
}

void returnEveryShip(game::World& world, PlayerRoster& roster) {
  for (const ConnectedPlayer* const player : roster.connected()) {
    if (auto* const position =
            world.shipComponentOf<game::Position>(player->shipEntityId)) {
      *position = game::shipStartPosition(player->playerNumber,
                                          network::MAXIMUM_PLAYERS);
    }
    if (auto* const motion =
            world.shipComponentOf<game::ShipMotion>(player->shipEntityId)) {
      *motion = game::ShipMotion{};
    }
  }
}

}  // namespace rtype::server
