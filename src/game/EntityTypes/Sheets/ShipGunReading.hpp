#pragma once

#include <span>
#include "EntitySheets.hpp"
#include "SheetText.hpp"
#include "ShipGun.hpp"

namespace rtype::game {

/**
 * @returns The gun of the ships of the players: the first sheet of side Player
 * that has a gun, what it fires, its ticks between shots and before the first
 * one, and the velocity of its missile. The projectile type and the velocity
 * are 0 when that missile is not of side PlayerMissile, steady and piloted by
 * straight: its path is then no straight line the client can predict.
 * Everything is 0 when no ship has a gun. The ticks are at most 3600 (60 s of
 * GUN_INTERVAL_RANGE and FIRST_SHOT_RANGE), so they fit a u16.
 */
[[nodiscard]] ShipGun shipGunOf(const EntitySheets& types);

/**
 * @returns The gun shipGunOf() reads from the sheets of @p texts, or one that
 * predicts nothing when they do not read as EntitySheets: a modded server may
 * name behaviours this program does not know, and its missiles then go
 * unpredicted rather than its content refused.
 */
[[nodiscard]] ShipGun shipGunOrNoneOf(std::span<const SheetText> texts);

}  // namespace rtype::game
