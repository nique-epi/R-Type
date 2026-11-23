#pragma once

#include <optional>
#include <vector>
#include "DataFileTable.hpp"
#include "GunSheet.hpp"
#include "WeaponSheet.hpp"

namespace rtype::game {

/**
 * @returns The gun of `[gameplay.gun]`, its durations in the nearest whole
 * ticks, at least MINIMUM_GUN_INTERVAL_TICKS between two shots, and the shape
 * of its shots; nothing without it. Its projectile is still a name, which
 * EntitySheets checks against the other sheets.
 * @throws What the engine::…At() readers throw for a key.
 */
[[nodiscard]] std::optional<GunSheet> gunFrom(
    const engine::DataFileTable& gameplay);

/**
 * @returns The weapons of the tables `[[gameplay.weapon]]` of @p gameplay, in
 * the order of the file, each with the guns of its levels; none without
 * them.
 * @throws What the engine::…At() readers throw for a key, and
 * InconsistentEntitySheetException past MOST_WEAPONS weapons, for a weapon of
 * no level or past MOST_WEAPON_LEVELS, or for two weapons of one name.
 */
[[nodiscard]] std::vector<WeaponSheet> weaponsFrom(
    const engine::DataFileTable& gameplay);

}  // namespace rtype::game
