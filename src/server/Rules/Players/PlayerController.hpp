#pragma once

#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Writes the Intent of the ship of @p player in @p world from @p input:
 * the directions and the shot it holds, that the ship keeps wanting until a
 * newer input arrives. Nothing when the player has no ship in that world.
 *
 * This is the player side of the control of a ship: MovementSystem and the
 * weapon system act on the Intent and never read an input. The Intent of a
 * ship is the last input of its player; a new ship wants nothing until the next
 * input arrives.
 */
void steerShipByInput(game::World& world, const ConnectedPlayer& player,
                      const network::PlayerInput& input);

/**
 * @brief Makes the ship of every player of @p roster who is away want nothing
 * in @p world: its player no longer holds anything.
 */
void silenceAwayShips(game::World& world, PlayerRoster& roster);

}  // namespace rtype::server
