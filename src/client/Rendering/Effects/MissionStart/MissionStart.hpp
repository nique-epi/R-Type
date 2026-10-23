#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns How many times their own speed the stars move at that time since
 * the start of the game: 1 before it; from 1 to WARP_TOP_SPEED_FACTOR in a
 * straight line over WARP_RISE; back to 1, fast then slowly (a cubic ease
 * out), at WARP_DURATION; 1 after.
 */
[[nodiscard]] float warpSpeedFactorAt(engine::Duration sinceStart);

}  // namespace rtype::client
