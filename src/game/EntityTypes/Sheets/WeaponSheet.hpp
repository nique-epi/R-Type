#pragma once

#include <string>
#include <vector>
#include "DataFilePlace.hpp"
#include "GunSheet.hpp"

namespace rtype::game {

/**
 * @brief One table `[[gameplay.weapon]]` of the sheet of a ship: a weapon a
 * crystal of its name gives the ship, and the gun of each of its levels, the
 * first level first.
 */
struct WeaponSheet {
  std::string name;
  /** @brief Where the file names the weapon, for the errors of its sheet. */
  engine::DataFilePlace place;
  std::vector<GunSheet> levels;
};

}  // namespace rtype::game
