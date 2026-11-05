#pragma once

namespace rtype::client {

/** @brief A distance in pixels of the canvas, the unit of the sprites of the
 * game: x to the right, y downwards. */
struct FrameAnchor {
  int x{0};
  int y{0};

  bool operator==(const FrameAnchor&) const = default;
};

}  // namespace rtype::client
