#pragma once

#include "ObstacleContact.hpp"

namespace rtype::game {

/** @brief Makes a body an obstacle: what a ship that touches it gets. */
struct Obstacle {
  ObstacleContact contact{ObstacleContact::Fatal};
};

}  // namespace rtype::game
