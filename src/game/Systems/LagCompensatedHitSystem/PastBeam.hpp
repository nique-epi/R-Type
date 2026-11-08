#pragma once

#include "BeamLine.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief A beam of the HurtZone of an enemy as it lay at one tick: the enemy
 * it belongs to and its line there. */
struct PastBeam {
  engine::Entity entity{};
  BeamLine line{};
};

}  // namespace rtype::game
