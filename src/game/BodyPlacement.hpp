#pragma once

#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a body appears, and the point its Pilot steers from: the same
 * point for a body that appears where it starts its path, another one for a
 * shard that flies away from the center of the projectile it came out of.
 */
struct BodyPlacement {
  Position position{};
  Position origin{};
};

}  // namespace rtype::game
