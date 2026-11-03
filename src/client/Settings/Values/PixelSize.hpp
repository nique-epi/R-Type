#pragma once

namespace rtype::client {

/**
 * @brief Size of a surface of the screen, in pixels: the drawing area of a
 * window, or the whole desktop.
 */
struct PixelSize {
  unsigned int width;
  unsigned int height;

  bool operator==(const PixelSize& other) const = default;
};

}  // namespace rtype::client
