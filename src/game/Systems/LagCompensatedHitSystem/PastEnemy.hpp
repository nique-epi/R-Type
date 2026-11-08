#pragma once

#include "Bounds.hpp"
#include "Entity.hpp"
#include "ThreatKind.hpp"

namespace rtype::game {

/** @brief A box of an enemy as it was at one tick: the entity it belongs to,
 * the box there, and what it does to what touches it. */
struct PastEnemy {
  engine::Entity entity{};
  engine::Bounds bounds{};
  ThreatKind kind{ThreatKind::Body};
};

}  // namespace rtype::game
