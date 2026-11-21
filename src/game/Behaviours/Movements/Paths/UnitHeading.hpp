#pragma once

#include "Direction.hpp"

namespace rtype::game {

/**
 * @returns The heading along @p offset at full speed: @p offset divided by its
 * length, so a body flies as fast along a diagonal as along a row; LEFTWARD,
 * the way of the missiles of an enemy, for an offset of no length.
 */
[[nodiscard]] Direction headingAlong(Direction offset);

}  // namespace rtype::game
