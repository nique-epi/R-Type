#pragma once

#include <cstddef>

namespace rtype::game {

class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/**
 * @brief Holds the ship of every player of @p roster in @p world for one tick
 * of a transition of the level: it wants nothing, so it neither moves nor
 * fires, its ShipMotion starts again from rest, so it cannot drift, roll or
 * dash, and it stays invulnerable for at least @p ticksLeft ticks, the end of
 * the transition. Nothing for a player without a ship.
 */
void holdEveryShip(game::World& world, PlayerRoster& roster,
                   std::size_t ticksLeft);

/**
 * @brief Puts the ship of every player of @p roster in @p world back at its
 * start, where a new ship of that player appears (game::shipStartPosition()),
 * at rest. Nothing for a player without a ship.
 */
void returnEveryShip(game::World& world, PlayerRoster& roster);

}  // namespace rtype::server
