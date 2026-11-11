#pragma once

#include "Bounds.hpp"
#include "CollisionFilter.hpp"
#include "Entity.hpp"
#include "ThreatKind.hpp"

namespace rtype::game {

/** @brief A box of an enemy as it was at one tick: the entity it belongs to,
 * the box there, what it does to what touches it, and the collision filter of
 * the entity then. */
struct PastEnemy {
  engine::Entity entity{};
  engine::Bounds bounds{};
  ThreatKind kind{ThreatKind::Body};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
