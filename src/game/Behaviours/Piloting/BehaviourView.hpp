#pragma once

#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief What a behaviour may know of the body it pilots, at one tick.
 */
struct BehaviourView {
  /** @brief Where the center of the body is now, before this tick moves it. */
  Position position{};
  /** @brief Where the center of the body was when it appeared. */
  Position origin{};
  /** @brief How long the body has existed, this tick included. */
  engine::Duration age{};
  /** @brief How long this tick lasts. */
  engine::Duration elapsed{};
  /** @brief The speed of the body on each axis at a full Direction, in logical
   * units per second; 0 for a body with no SteadyMotion. */
  float topSpeed{};
};

}  // namespace rtype::game
