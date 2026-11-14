#pragma once

#include "CollisionFilter.hpp"
#include "ContactShape.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An entity, the shape it is touched on during this update (one box
 * of it, and its mask laid where it is), and its collision filter, as a system
 * lists the entities it tests. */
struct EntityShape {
  engine::Entity entity{};
  ContactShape shape{};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
