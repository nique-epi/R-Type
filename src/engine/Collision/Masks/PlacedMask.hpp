#pragma once

#include "PixelMask.hpp"

namespace rtype::engine {

/** @brief A PixelMask laid on a grid of pixels: the mask, never null, which
 * outlives the placement, and the column and the row of the grid its top-left
 * pixel covers. Masks compared with one another lie on the same grid. */
struct PlacedMask {
  const PixelMask* mask{};
  int left{};
  int top{};
};

}  // namespace rtype::engine
