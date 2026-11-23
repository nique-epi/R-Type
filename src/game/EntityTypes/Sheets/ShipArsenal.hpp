#pragma once

#include <vector>
#include "ShipGun.hpp"

namespace rtype::game {

/**
 * @brief Every gun the ships of the players may fire, as shipArsenalOf()
 * reads them from the sheets the session plays, so the client can fire its
 * own missiles before the server answers: the gun of the sheet, and the gun
 * of each level of each of its weapons, the n-th weapon at weapons[n - 1], its
 * first level first. A weapon the client cannot predict holds guns that name
 * no projectile (see ShipGun).
 */
struct ShipArsenal {
  ShipGun gun;
  std::vector<std::vector<ShipGun>> weapons;

  bool operator==(const ShipArsenal&) const = default;
};

}  // namespace rtype::game
