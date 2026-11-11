#pragma once

#include "BeamLine.hpp"
#include "CollisionFilter.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief A beam of the HurtZone of an enemy as it lay at one tick: the enemy
 * it belongs to, its line there and the collision filter of the enemy then. */
struct PastBeam {
  engine::Entity entity{};
  BeamLine line{};
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
