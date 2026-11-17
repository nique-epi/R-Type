#pragma once

#include "CollisionFilter.hpp"
#include "ContactShape.hpp"
#include "Entity.hpp"
#include "ThreatKind.hpp"

namespace rtype::game {

/** @brief A box of an enemy as it was at one tick: the entity it belongs to,
 * the shape it was touched on there (the box, and its CollisionMask laid where
 * it stood then), what it does to what touches it, and the collision filter
 * of the entity then. */
struct PastEnemy {
  engine::Entity entity{};
  ContactShape shape{};
  ThreatKind kind{ThreatKind::Body};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
