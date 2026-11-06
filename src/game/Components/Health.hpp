#pragma once

namespace rtype::game {

/**
 * @brief Health points an entity has left. The entity is destroyed when they
 * reach 0 or less.
 */
struct Health {
  int points{0};
  /** @brief The points it appeared with, which tell a wounded body from an
   * untouched one; 0 for a body made with no such count. */
  int full{0};
};

}  // namespace rtype::game
