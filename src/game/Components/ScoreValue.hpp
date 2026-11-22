#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The points the player who destroys this entity earns, and the
 * kopecks every pilot present earns then: the bounty of a boss, 0 for any
 * other body.
 */
struct ScoreValue {
  std::uint32_t points{0};
  std::uint32_t bounty{0};
};

}  // namespace rtype::game
