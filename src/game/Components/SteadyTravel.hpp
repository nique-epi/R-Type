#pragma once

#include <algorithm>
#include <chrono>
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Position.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/** @returns How far a body of @p motion steered to @p direction moves along
 * each axis during @p elapsed: its top speed times the share @p direction asks
 * for, brought within -FULL_STEERING and FULL_STEERING. */
constexpr Position steadyTravel(const SteadyMotion& motion, Direction direction,
                                engine::Duration elapsed) {
  const float reach =
      motion.topSpeed * std::chrono::duration<float>(elapsed).count();
  return Position{
      .x = std::clamp(direction.x, -FULL_STEERING, FULL_STEERING) * reach,
      .y = std::clamp(direction.y, -FULL_STEERING, FULL_STEERING) * reach};
}

}  // namespace rtype::game
