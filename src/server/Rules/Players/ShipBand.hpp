#pragma once

#include "PlayBand.hpp"

namespace rtype::game {

class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/**
 * @brief Gives the ship of every player of @p roster in @p world the band
 * @p band, the rows the biome the level shows leaves free, so MovementSystem
 * stops it at its ceiling and its floor. Nothing for a player without a ship.
 */
void bandEveryShip(game::World& world, PlayerRoster& roster,
                   game::PlayBand band);

}  // namespace rtype::server
