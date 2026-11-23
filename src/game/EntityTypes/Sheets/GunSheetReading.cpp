#include "GunSheetReading.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "SheetTicks.hpp"
#include "ShotPattern.hpp"
#include "WeaponSheet.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

namespace {

/** @returns The value of @p key of @p table in logical units or degrees, 0
 * when it holds none. */
float unitsOrZeroAt(const engine::DataFileTable& table, std::string_view key,
                    engine::WholeNumberRange range) {
  return static_cast<float>(engine::wholeNumberOrZeroAt(table, key, range));
}

/** @returns The shape of the shots of the gun of @p table: one projectile
 * straight ahead at the muzzle for the keys it leaves out. */
ShotPattern patternAt(const engine::DataFileTable& table) {
  return ShotPattern{
      .count = static_cast<std::uint8_t>(std::max<std::int64_t>(
          1,
          engine::wholeNumberOrZeroAt(table, COUNT_KEY, GUN_SHOT_COUNT_RANGE))),
      .spreadDegrees = unitsOrZeroAt(table, SPREAD_KEY, GUN_SPREAD_RANGE),
      .stepY = unitsOrZeroAt(table, STEP_Y_KEY, GUN_STEP_RANGE),
      .distance = unitsOrZeroAt(table, DISTANCE_KEY, GUN_DISTANCE_RANGE)};
}

/** @returns The gun of @p table, which waits @p firstShotTicks before its
 * first shot. */
GunSheet gunAt(const engine::DataFileTable& table, std::size_t firstShotTicks) {
  return GunSheet{
      .intervalTicks = std::max(MINIMUM_GUN_INTERVAL_TICKS,
                                nearestTicksIn(engine::millisecondsAt(
                                    table, INTERVAL_KEY, GUN_INTERVAL_RANGE))),
      .firstShotTicks = firstShotTicks,
      .projectile = engine::textAt(table, PROJECTILE_KEY),
      .projectileNumber = 0,
      .projectilePlace = engine::placeOfKey(table, PROJECTILE_KEY),
      .pattern = patternAt(table)};
}

/** @returns The gun of one level of a weapon, ready at once.
 * @throws What the engine::…At() readers throw for a key. */
GunSheet levelOf(const engine::DataFileTable& table) {
  engine::refuseUnknownKeys(table, WEAPON_LEVEL_KEYS);
  return gunAt(table, 0);
}

/** @returns The weapon one table of `[[gameplay.weapon]]` describes.
 * @throws InconsistentEntitySheetException for no level or past
 * MOST_WEAPON_LEVELS. */
WeaponSheet weaponFrom(const engine::DataFileTable& table) {
  engine::refuseUnknownKeys(table, WEAPON_KEYS);
  WeaponSheet weapon{.name = engine::textAt(table, WEAPON_NAME_KEY),
                     .place = engine::placeOfKey(table, WEAPON_NAME_KEY),
                     .levels = {}};
  for (const engine::DataFileTable& level :
       engine::tablesAt(table, WEAPON_LEVEL_KEY)) {
    weapon.levels.push_back(levelOf(level));
  }
  if (weapon.levels.empty() || weapon.levels.size() > MOST_WEAPON_LEVELS) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(table, WEAPON_LEVEL_KEY), WEAPON_LEVEL_RULE);
  }
  return weapon;
}

/** @throws InconsistentEntitySheetException when @p weapons already holds a
 * weapon named as @p weapon. */
void refuseRepeatedName(const std::vector<WeaponSheet>& weapons,
                        const WeaponSheet& weapon) {
  if (std::ranges::any_of(weapons, [&weapon](const WeaponSheet& held) {
        return held.name == weapon.name;
      })) {
    throw InconsistentEntitySheetException(weapon.place, WEAPON_NAME_RULE);
  }
}

}  // namespace

std::optional<GunSheet> gunFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> gun =
      engine::optionalTableAt(gameplay, GUN_KEY);
  if (!gun.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*gun, GUN_KEYS);
  return gunAt(*gun, nearestTicksIn(engine::millisecondsAt(*gun, FIRST_SHOT_KEY,
                                                           FIRST_SHOT_RANGE)));
}

std::vector<WeaponSheet> weaponsFrom(const engine::DataFileTable& gameplay) {
  if (!gameplay.table.get().contains(WEAPON_KEY)) {
    return {};
  }
  const std::vector<engine::DataFileTable> tables =
      engine::tablesAt(gameplay, WEAPON_KEY);
  if (tables.size() > MOST_WEAPONS) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, WEAPON_KEY), WEAPON_COUNT_RULE);
  }
  std::vector<WeaponSheet> weapons;
  for (const engine::DataFileTable& table : tables) {
    const WeaponSheet weapon = weaponFrom(table);
    refuseRepeatedName(weapons, weapon);
    weapons.push_back(weapon);
  }
  return weapons;
}

}  // namespace rtype::game
