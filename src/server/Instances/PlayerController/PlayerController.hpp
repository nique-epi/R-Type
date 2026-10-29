#pragma once

#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Writes the Intent of the ship of every connected player of
 * @p roster in @p world: the directions its newest input holds, or none when
 * the player sent no input. A player without a ship in that world is skipped.
 *
 * This is the player side of the control of a ship: MovementSystem moves the
 * ship by its Intent and never reads an input.
 */
void steerPlayerShips(game::World& world, PlayerRoster& roster);

}  // namespace rtype::server
