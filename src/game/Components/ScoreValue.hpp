#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The points the player who destroys this entity earns.
 */
struct ScoreValue {
  std::uint32_t points{0};
};

}  // namespace rtype::game
