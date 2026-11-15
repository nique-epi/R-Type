#pragma once

namespace rtype::client {

/** @brief How a line is laid on a texture: how many texture pixels make one
 * pixel of the picture, and how many make one pixel of its font. */
struct TextPlacement {
  int density{};
  int fontPixels{};
};

}  // namespace rtype::client
