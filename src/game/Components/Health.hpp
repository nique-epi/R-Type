#pragma once

namespace rtype::game {

/**
 * @brief Health points an entity has left. The entity is destroyed when they
 * reach 0 or less.
 */
struct Health {
  int points{0};
};

}  // namespace rtype::game
