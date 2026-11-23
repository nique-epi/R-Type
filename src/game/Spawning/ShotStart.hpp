#pragma once

#include "EntityType.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where one shot of a gun starts: the start of its single projectile
 * (see projectileStartPosition()), and the side of the body that fires it,
 * which says the way it fires.
 */
struct ShotStart {
  Position point{};
  EntityType side{EntityType::Player};
};

}  // namespace rtype::game
