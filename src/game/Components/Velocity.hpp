#pragma once

namespace rtype::game {

/**
 * @brief How fast an entity moves, in logical units per second.
 *
 * A positive x moves to the right, a positive y moves downwards.
 */
struct Velocity {
  float x;
  float y;
};

}  // namespace rtype::game
