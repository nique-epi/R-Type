#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The figure of a gain rises POPUP_RISE_HEIGHT pixels of the canvas
 * over POPUP_RISE, stays POPUP_HOLD, blinks POPUP_BLINK_COUNT times,
 * POPUP_BLINK_STEP per state, and goes after POPUP_LIFE.
 */
constexpr engine::Duration POPUP_RISE = std::chrono::milliseconds(400);
constexpr engine::Duration POPUP_HOLD = std::chrono::milliseconds(300);
constexpr engine::Duration POPUP_BLINK_STEP = std::chrono::milliseconds(67);
constexpr int POPUP_BLINK_COUNT = 2;
constexpr int POPUP_RISE_HEIGHT = 8;
constexpr engine::Duration POPUP_LIFE =
    POPUP_RISE + POPUP_HOLD + (2 * POPUP_BLINK_COUNT * POPUP_BLINK_STEP);

}  // namespace rtype::client
