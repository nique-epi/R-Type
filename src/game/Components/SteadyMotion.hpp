#pragma once

namespace rtype::game {

/**
 * @brief Component of a body that moves at a fixed speed: on each axis, its
 * top speed times the share its Intent asks for, with no drift and no
 * maneuver.
 */
struct SteadyMotion {
  /** @brief The speed on each axis at a full Direction, in logical units per
   * second. */
  float topSpeed{};
};

}  // namespace rtype::game
