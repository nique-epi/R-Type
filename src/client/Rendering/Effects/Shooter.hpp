#pragma once

#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::client {

/** @brief What fired a missile, the ship of a player or a Bydo, and where it
 * stands. */
struct Shooter {
  engine::Entity ship{};
  game::Position center{};
};

}  // namespace rtype::client
