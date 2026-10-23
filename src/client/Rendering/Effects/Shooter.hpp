#pragma once

#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::client {

/** @brief The ship a missile of a player came from, and where it stands. */
struct Shooter {
  engine::Entity ship{};
  game::Position center{};
};

}  // namespace rtype::client
