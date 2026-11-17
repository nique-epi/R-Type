#include "MaskFixtures.hpp"
#include <cstddef>
#include <span>
#include <string_view>
#include "MaskPixel.hpp"
#include "MaskTestConstants.hpp"
#include "PixelMask.hpp"

using rtype::engine::MaskPixel;
using rtype::engine::PixelMask;

PixelMask maskFromRows(std::span<const std::string_view> rows) {
  PixelMask mask(static_cast<int>(rows.front().size()),
                 static_cast<int>(rows.size()));
  for (std::size_t line = 0; line < rows.size(); ++line) {
    for (std::size_t column = 0; column < rows[line].size(); ++column) {
      if (rows[line][column] == DRAWN_TEST_PIXEL) {
        mask.markDrawn(
            {.x = static_cast<int>(column), .y = static_cast<int>(line)});
      }
    }
  }
  return mask;
}

PixelMask fullMask(int width, int height) {
  PixelMask mask(width, height);
  for (int line = 0; line < height; ++line) {
    for (int column = 0; column < width; ++column) {
      mask.markDrawn({.x = column, .y = line});
    }
  }
  return mask;
}

PixelMask rowDrawnAt(int width, MaskPixel pixel) {
  PixelMask mask(width, 1);
  mask.markDrawn(pixel);
  return mask;
}
