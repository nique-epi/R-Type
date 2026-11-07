#pragma once

#include "MotionKind.hpp"

namespace rtype::game {

/** @brief The table `[gameplay.motion]` of a sheet. */
struct MotionSheet {
  MotionKind kind{MotionKind::Steady};
  /** @brief The speed on each axis at a full direction, in logical units per
   * second; 0 for a ship, whose speed is the one of shipAfter(). */
  float topSpeed{};
};

}  // namespace rtype::game
