#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief MISSION START: from the start of the game, the stars reach
 * WARP_TOP_SPEED_FACTOR times their speed in WARP_RISE, 7 frames, then slow
 * down to their own speed, fast then slowly, until WARP_DURATION, 36 frames.
 */
constexpr engine::Duration WARP_RISE = std::chrono::milliseconds(117);
constexpr engine::Duration WARP_DURATION = std::chrono::milliseconds(600);
constexpr float WARP_TOP_SPEED_FACTOR = 12.0F;

/** @brief A ship that appears earlier than PILOT_ARRIVAL_DELAY into the game
 * arrives then, while the stars slow down. */
constexpr engine::Duration PILOT_ARRIVAL_DELAY = std::chrono::milliseconds(500);

}  // namespace rtype::client
