#pragma once

namespace rtype::game {

/** @brief A point of a barrel roll seen on a frame: the height of the frame,
 * in rows, and how far through the roll the ship is, from 0 to 1. */
struct RollMoment {
  int frameHeight{};
  float share{};
};

}  // namespace rtype::game
