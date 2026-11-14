#pragma once

namespace rtype::engine {

/** @brief One pixel of a PixelMask: its column from the left edge and its row
 * from the top edge, from 0. */
struct MaskPixel {
  int x{};
  int y{};
};

}  // namespace rtype::engine
