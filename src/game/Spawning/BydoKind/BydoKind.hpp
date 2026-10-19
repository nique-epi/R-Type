#pragma once

#include <cstdint>
#include "MovementPattern.hpp"

namespace rtype::game {

/**
 * @brief The kinds of Bydo; each one moves along its own pattern.
 */
enum class BydoKind : std::uint8_t {
  Drifter,
  Weaver,
};

/**
 * @returns The pattern shape the Bydos of @p kind follow: Drifter goes
 * straight, Weaver goes in a wave.
 */
[[nodiscard]] PatternShape patternShapeOf(BydoKind kind);

}  // namespace rtype::game
