#pragma once

#include <span>
#include "EntitySheets.hpp"
#include "HeldWeapon.hpp"
#include "SheetText.hpp"
#include "ShipArsenal.hpp"
#include "ShipGun.hpp"

namespace rtype::game {

/**
 * @returns The guns of the ships of the players: those of the first sheet of
 * side Player that has a gun, its own and each level of each of its weapons,
 * each with what it fires, its ticks between shots and before the first one,
 * how its shot is laid, and how its missile flies. The projectile type and
 * the flight of a gun are 0 when its missile is not of side PlayerMissile,
 * steady and piloted by straight or scatter: its path is then no line the
 * client can predict. Everything is 0 when no ship has a gun. The ticks are at
 * most 3600 (60 s of GUN_INTERVAL_RANGE and FIRST_SHOT_RANGE), so they fit a
 * u16.
 */
[[nodiscard]] ShipArsenal shipArsenalOf(const EntitySheets& types);

/**
 * @returns The guns shipArsenalOf() reads from the sheets of @p texts, or none
 * that predicts anything when they do not read as EntitySheets: a modded
 * server may name behaviours this program does not know, and its missiles
 * then go unpredicted rather than its content refused.
 */
[[nodiscard]] ShipArsenal shipArsenalOrNoneOf(std::span<const SheetText> texts);

/**
 * @returns The gun of @p arsenal a ship holding @p held fires: the level of
 * the weapon it names, or the gun of the sheet for weapon 0 and for a weapon
 * or a level @p arsenal does not hold.
 */
[[nodiscard]] const ShipGun& heldGunOf(const ShipArsenal& arsenal,
                                       HeldWeapon held);

}  // namespace rtype::game
