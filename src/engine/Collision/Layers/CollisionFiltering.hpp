#pragma once

#include <cstddef>
#include <cstdint>
#include "CollisionFilter.hpp"
#include "CollisionLayerConstants.hpp"

namespace rtype::engine {

/** @returns The set that holds only the collision layer of @p index, counted
 * from 0 and below MAXIMUM_COLLISION_LAYERS. */
constexpr std::uint32_t collisionLayerOf(std::size_t index) {
  return FIRST_COLLISION_LAYER << index;
}

/** @returns Whether @p striker hits a layer @p struck lies in. */
constexpr bool hitsLayerOf(const CollisionFilter& striker,
                           const CollisionFilter& struck) {
  return (striker.hits & struck.layer) != NO_COLLISION_LAYER;
}

/**
 * @brief Tells whether two entities are worth testing against each other: one
 * of them hits a layer the other lies in. The result is the same whichever
 * filter comes first.
 */
constexpr bool mayCollide(const CollisionFilter& first,
                          const CollisionFilter& second) {
  return hitsLayerOf(first, second) || hitsLayerOf(second, first);
}

}  // namespace rtype::engine
