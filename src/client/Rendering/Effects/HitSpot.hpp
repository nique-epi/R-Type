#pragma once

#include "EntityType.hpp"
#include "Position.hpp"

namespace rtype::client {

/** @brief What an entity hit is, and where it stands. */
struct HitSpot {
  game::EntityType type{};
  game::Position center{};
};

}  // namespace rtype::client
