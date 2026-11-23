#pragma once

#include <cstdint>
#include "Muzzle.hpp"
#include "ShotPattern.hpp"

namespace rtype::server {

/**
 * @brief A Gun that fires at this tick: the body that fires, the number of the
 * sheet of what it fires, the network id of the body, and the shape of the
 * shot.
 */
struct WeaponShot {
  game::Muzzle muzzle{};
  std::uint8_t projectileNumber{0};
  std::uint32_t shooterId{0};
  game::ShotPattern pattern{};
};

}  // namespace rtype::server
