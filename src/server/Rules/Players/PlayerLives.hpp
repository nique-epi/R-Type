#pragma once

#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @returns The lives @p player has left in @p world, the ship in play
 * included: the Lives of its ship while it has one in that world, else the
 * lives it keeps for its next ship (ConnectedPlayer::livesWithoutShip).
 */
[[nodiscard]] std::uint8_t livesOf(game::World& world,
                                   const ConnectedPlayer& player);

}  // namespace rtype::server
