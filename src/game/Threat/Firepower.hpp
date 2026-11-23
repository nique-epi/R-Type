#pragma once

#include "ComponentRegistry.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "TeamFirepower.hpp"

namespace rtype::game {

/**
 * @returns What @p held fires in percent of what @p sheetGun fires: the
 * projectiles of a shot that fly within FORWARD_CONE_DEGREES of the heading
 * of the ship, over the ticks between two shots, rounded down. The gun of the
 * sheet gives WHOLE_PERCENT, and so does any gun when @p sheetGun fires
 * nothing forward. Every projectile counts once: none hurts more than another.
 */
[[nodiscard]] int firepowerPercentOf(const Gun& held, const GunSheet& sheetGun);

/**
 * @returns The most a gun of @p ship fires, its own or a level of one of its
 * weapons, in percent of its own (see firepowerPercentOf()); WHOLE_PERCENT
 * for a sheet without a gun.
 */
[[nodiscard]] int highestFirepowerPercentOf(const EntitySheet& ship);

/**
 * @returns What the ships of a team fire (see TeamFirepower): every body of
 * side player in @p components that holds a Gun, read against the sheet
 * @p sheets numbers it with. A body whose sheet has no gun is left out.
 */
[[nodiscard]] TeamFirepower teamFirepowerOf(
    engine::ComponentRegistry& components, const EntitySheets& sheets);

}  // namespace rtype::game
