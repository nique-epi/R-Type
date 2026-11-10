#pragma once

#include "Bounds.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An entity and the rectangle it collides with on this update, in
 * logical units, as a system lists the entities it tests. */
struct EntityBounds {
  engine::Entity entity;
  engine::Bounds bounds;
};

}  // namespace rtype::game
