#pragma once

#include <cstdint>

namespace rtype::engine {

/**
 * @brief Where an entity lies and what it hits, as two sets of collision
 * layers, one bit per layer (see collisionLayerOf()).
 *
 * Attached as a component next to the box of an entity. Two entities are
 * tested against each other only when mayCollide() says so: the layers carry
 * no name and no meaning here, the game gives them both. An entity with an
 * empty filter lies nowhere and hits nothing.
 */
struct CollisionFilter {
  /** @brief The layers the entity lies in. */
  std::uint32_t layer{};
  /** @brief The layers the entity hits. */
  std::uint32_t hits{};

  bool operator==(const CollisionFilter&) const = default;
};

}  // namespace rtype::engine
