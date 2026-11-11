#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::engine {

/**
 * @brief How many collision layers a program may name: a CollisionFilter
 * keeps one bit per layer in 32 bits.
 */
constexpr std::size_t MAXIMUM_COLLISION_LAYERS = 32;

/** @brief The set of collision layers that holds none. */
constexpr std::uint32_t NO_COLLISION_LAYER = 0;

/** @brief The set that holds the first collision layer only, the one of
 * index 0. */
constexpr std::uint32_t FIRST_COLLISION_LAYER = 1;

}  // namespace rtype::engine
