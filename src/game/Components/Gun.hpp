#pragma once

#include <cstddef>

namespace rtype::game {

/**
 * @brief Lets an entity fire missiles of its own, one every shot interval.
 *
 * Only the Bydos carry one. At each tick of a game in progress, the server
 * counts one tick on it; each time the count reaches the shot interval, the
 * entity fires a missile and the count starts again from 0.
 */
struct Gun {
  /** @brief Simulation ticks since the entity last fired, or since it appeared
   * when it never did. */
  std::size_t ticksSinceLastShot{0};
};

}  // namespace rtype::game
