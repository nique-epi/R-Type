#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief How a body turns the direction of its Intent into movement, as the
 * Entity types section of the architecture page lists them.
 */
enum class MotionKind : std::uint8_t {
  /** @brief The drift, roll and dash of shipAfter(): the ships. */
  Ship,
  /** @brief Its top speed times the direction, on each axis: every other
   * body. */
  Steady,
};

}  // namespace rtype::game
