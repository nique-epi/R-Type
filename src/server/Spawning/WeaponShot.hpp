#pragma once

#include <cstdint>
#include "Muzzle.hpp"

namespace rtype::server {

/**
 * @brief A Gun that fires at this tick: the body that fires, the number of the
 * sheet of what it fires, and the network id of the body.
 */
struct WeaponShot {
  game::Muzzle muzzle{};
  std::uint8_t projectileNumber{0};
  std::uint32_t shooterId{0};
};

}  // namespace rtype::server
