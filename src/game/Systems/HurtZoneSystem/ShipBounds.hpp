#pragma once

#include "Bounds.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief A ship and the rectangle it occupies, in logical units. */
struct ShipBounds {
  engine::Entity entity;
  engine::Bounds bounds;
};

}  // namespace rtype::game
