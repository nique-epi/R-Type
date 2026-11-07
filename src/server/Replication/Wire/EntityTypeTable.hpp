#pragma once

#include <vector>
#include "EntitySheets.hpp"
#include "EntityTypeEntry.hpp"
#include "ShipGun.hpp"

namespace rtype::server {

/**
 * @returns The table of @p types as ConnectionAccepted carries it: one entry
 * per sheet, in the order of their numbers, which is the order of their names,
 * each with its side, its name and its box, so entry n - 1 is the sheet
 * numbered n. A side of a box is at most 4096 units (HITBOX_SIDE_RANGE), so it
 * fits the u16 of the wire.
 */
[[nodiscard]] std::vector<network::EntityTypeEntry> entityTypeTableOf(
    const game::EntitySheets& types);

/**
 * @returns The gun of the ships of the players as ConnectionAccepted carries
 * it: the first sheet of side Player that has a gun, what it fires, its ticks
 * between shots and before the first one, and the velocity of its missile.
 * The projectile type and the velocity are 0 when that missile is not of side
 * PlayerMissile, steady and piloted by straight: its path is then no straight
 * line the client can predict. Everything is 0 when no ship has a gun. The
 * ticks are at most 3600 (60 s of GUN_INTERVAL_RANGE and FIRST_SHOT_RANGE), so
 * they fit the u16 of the wire.
 */
[[nodiscard]] network::ShipGun shipGunOf(const game::EntitySheets& types);

}  // namespace rtype::server
