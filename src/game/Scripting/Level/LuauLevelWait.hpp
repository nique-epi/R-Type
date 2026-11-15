#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief What the stage of a level waits for: the ticks left before it may
 * go on, one at least, and the scroll position the level must have reached
 * by then, 0 for a wait of time alone. */
struct LuauLevelWait {
  std::uint32_t ticks{1};
  float untilScrollPosition{0.0F};
};

}  // namespace rtype::game
