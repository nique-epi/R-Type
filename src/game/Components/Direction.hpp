#pragma once

namespace rtype::game {

/**
 * @brief Where a pilot wants its body to go, as a share of the full speed of
 * the body on each axis, from -FULL_STEERING to FULL_STEERING.
 *
 * A positive x goes right, a positive y goes down, as Position does. A player
 * writes -1, 0 or 1 on each axis; a behaviour may write any share between.
 */
struct Direction {
  float x{};
  float y{};
};

}  // namespace rtype::game
