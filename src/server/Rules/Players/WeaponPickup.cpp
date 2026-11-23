#include "WeaponPickup.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include "EntitySheet.hpp"
#include "Gun.hpp"
#include "GunSheet.hpp"
#include "WeaponSheet.hpp"

namespace rtype::server {

namespace {

/** @returns Where the weapon named @p weapon stands among the weapons of
 * @p ship; nothing when it holds none of that name. */
std::optional<std::size_t> placeOfWeapon(const game::EntitySheet& ship,
                                         std::string_view weapon) {
  const auto found = std::ranges::find_if(
      ship.weapons,
      [weapon](const game::WeaponSheet& held) { return held.name == weapon; });
  if (found == ship.weapons.end()) {
    return std::nullopt;
  }
  return static_cast<std::size_t>(found - ship.weapons.begin());
}

/** @returns The level a ship holding @p held reaches with a crystal of the
 * weapon numbered @p number, which has @p levels levels. */
std::size_t levelAfter(const game::Gun& held, std::uint8_t number,
                       std::size_t levels) {
  if (held.weapon != number) {
    return 1;
  }
  return std::min<std::size_t>(std::size_t{held.level} + 1, levels);
}

}  // namespace

std::optional<game::Gun> gunAfterPickup(const game::Gun& held,
                                        const game::EntitySheet& ship,
                                        std::string_view weapon) {
  const std::optional<std::size_t> place = placeOfWeapon(ship, weapon);
  if (!place.has_value()) {
    return std::nullopt;
  }
  const auto number = static_cast<std::uint8_t>(*place + 1);
  const std::size_t level =
      levelAfter(held, number, ship.weapons.at(*place).levels.size());
  const game::GunSheet& sheet = ship.weapons.at(*place).levels.at(level - 1);
  return game::Gun{
      .intervalTicks = sheet.intervalTicks,
      .ticksUntilReady = std::min(held.ticksUntilReady, sheet.intervalTicks),
      .projectileNumber = sheet.projectileNumber,
      .pattern = sheet.pattern,
      .weapon = number,
      .level = static_cast<std::uint8_t>(level)};
}

}  // namespace rtype::server
