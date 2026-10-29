#pragma once

#include <cstdint>
#include "EntityType.hpp"
#include "Position.hpp"

namespace rtype::server {

/**
 * @brief A Gun that fires at this tick: where its body is, what it fires, and
 * the network id of the body.
 */
struct WeaponShot {
  game::Position muzzle{};
  game::EntityType projectile{game::EntityType::PlayerMissile};
  std::uint32_t shooterId{0};
};

}  // namespace rtype::server
