#pragma once

#include <cstdint>

namespace rtype::engine {

/**
 * @brief Handle to a game object.
 *
 * The index names a slot of the registry and is reused once its entity is
 * destroyed. The generation tells apart the successive entities that lived in
 * that slot, so a handle kept after a destruction does not match the entity
 * that took its place.
 *
 * The generation is 32 bits wide and wraps around: a handle would match again
 * after its slot has been recycled 2^32 times. At 60 recycles per second of
 * one single slot, that takes more than two years.
 */
struct Entity {
  std::uint32_t index;
  std::uint32_t generation;

  bool operator==(const Entity&) const = default;
};

}  // namespace rtype::engine
