#pragma once

#include "PixelPosition.hpp"
#include "PixelSize.hpp"

namespace rtype::client {

/** @brief Where the window lies on the desktop: its size, and the position of
 * its top-left corner, in pixels of the desktop. */
struct WindowFrame {
  PixelSize size{};
  PixelPosition position{};
};

}  // namespace rtype::client
