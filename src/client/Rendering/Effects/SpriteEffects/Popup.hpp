#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of the figure of a gain, an entity with a Position and a
 * Caption: hidden while `age` is negative, then it rises POPUP_RISE_HEIGHT
 * pixels of the canvas above `originY` over POPUP_RISE, fast then slowly,
 * stays POPUP_HOLD, blinks POPUP_BLINK_COUNT times, POPUP_BLINK_STEP per
 * state, and goes. SpriteEffectSystem moves it and destroys it.
 */
struct Popup {
  float originY{};
  engine::Duration age{};
};

}  // namespace rtype::client
