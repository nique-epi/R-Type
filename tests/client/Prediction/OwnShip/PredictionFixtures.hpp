#pragma once

#include <cstdint>
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipDirections.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/** @returns The ship of the server after it received @p count commands
 * holding @p held from rest at START: commands numbered from 1, each followed
 * by a tick. */
game::MovingShip shipAfterCommands(game::ShipDirections held,
                                   std::uint32_t count);

/** @returns @p ship moved by @p offsetX along the x axis. */
game::Position shiftedRight(game::Position ship, float offsetX);

/** @returns A world state of tick 1 with the ship of @p shipId for the player
 * @p playerNumber at @p position, confirming @p acknowledgedInput. */
network::WorldState stateWithShip(std::uint8_t playerNumber,
                                  std::uint32_t shipId, game::Position position,
                                  std::uint32_t acknowledgedInput);

/** @returns A state that lists the player @p playerNumber without a ship, as
 * once its ship was destroyed, the server having applied its inputs up to
 * @p acknowledgedInput. */
network::WorldState stateWithoutAShip(std::uint8_t playerNumber,
                                      std::uint32_t acknowledgedInput);

/** @returns A state that lists the player @p playerNumber and its ship id but
 * no entity, as another part of a state cut in several datagrams does. */
network::WorldState stateWithoutEntities(std::uint8_t playerNumber,
                                         std::uint32_t shipId);

}  // namespace rtype::client::testing
