#pragma once

#include <vector>
#include "Collision.hpp"
#include "CollisionScene.hpp"
#include "CollisionSystem.hpp"

namespace rtype::game::testing {

/** @returns The collisions @p system publishes on the bus of @p scene in one
 * update, once the bus dispatched them. */
std::vector<engine::Collision> updateAndCollect(CollisionScene& scene,
                                                CollisionSystem& system);

}  // namespace rtype::game::testing
