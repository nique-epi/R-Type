#pragma once

#include "Bounds.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An entity CollisionSystem tests, with the rectangle it collides with
 * on this update. */
struct CollisionCandidate {
  engine::Entity entity;
  engine::Bounds bounds;
};

}  // namespace rtype::game
