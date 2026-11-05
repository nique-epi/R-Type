#pragma once

#include "CollisionBox.hpp"
#include "Muzzle.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a projectile of box @p projectile fired by @p muzzle appears:
 * on the row of the body, its back against the front of the body. A body of
 * the side EntityType::Player faces right, every other body faces left, so a
 * ship fires from its nose and an enemy from its left side.
 */
[[nodiscard]] Position projectileStartPosition(const Muzzle& muzzle,
                                               CollisionBox projectile);

}  // namespace rtype::game
