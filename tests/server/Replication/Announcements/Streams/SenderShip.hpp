#pragma once

#include <cstdint>
#include "Gun.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"

namespace rtype::server::testing {

/** @brief The ship of a player a world state test adds: its network id, its
 * lives, its protection and its gun, the gun of a sheet by default. */
struct SenderShip {
  std::uint32_t networkId{0};
  game::Lives lives{};
  game::Invulnerable protection{};
  game::Gun gun{};
};

}  // namespace rtype::server::testing
