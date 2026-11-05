#pragma once

#include "CollisionBox.hpp"
#include "EntityType.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief A body that fires: where its center is, the box it collides with, and
 * its side, which says the way it faces.
 */
struct Muzzle {
  Position center{};
  CollisionBox box{};
  EntityType side{EntityType::Player};
};

}  // namespace rtype::game
