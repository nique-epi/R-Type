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
/**
 * @brief Gives @p player @p lives in @p world: the Lives of its ship while it
 * has one, and the lives it keeps for its next ship, which a ship is given
 * and holds the same count as (ConnectedPlayer::livesWithoutShip).
 */
void setLivesOf(game::World& world, ConnectedPlayer& player,
                std::uint8_t lives);

[[nodiscard]] std::uint8_t livesOf(game::World& world,
                                   const ConnectedPlayer& player);

}  // namespace rtype::server
