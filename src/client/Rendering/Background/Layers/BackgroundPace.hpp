#pragma once

namespace rtype::client {

/** @brief What moves the layers of a background besides the time: how far
 * the level scrolled since the background began, in background pixels, and
 * how many times their own speed the layers that warp move by themselves. */
struct BackgroundPace {
  float scrolled{};
  float speedFactor{1.0F};
};

}  // namespace rtype::client
