#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief A transition a level plays between two of its parts: the ticks it
 * has left. While it lasts the scroll is paused and the server holds every
 * ship; LEVEL_TRANSITION_RETURN_TICKS before its end, it puts them back at
 * their start.
 */
struct LevelTransition {
  std::uint32_t ticksLeft{};
};

}  // namespace rtype::game
