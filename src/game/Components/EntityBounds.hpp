#pragma once

#include "Bounds.hpp"
#include "CollisionFilter.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An entity, the rectangle it collides with on this update, in
 * logical units, and its collision filter, as a system lists the entities it
 * tests. */
struct EntityBounds {
  engine::Entity entity{};
  engine::Bounds bounds{};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
