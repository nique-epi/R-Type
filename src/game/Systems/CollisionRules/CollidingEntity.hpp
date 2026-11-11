#pragma once

#include "CollisionFilter.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"

namespace rtype::game {

/** @brief One of the two entities of a collision, with its side and its
 * collision filter. */
struct CollidingEntity {
  engine::Entity entity{};
  EntityType type{};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
