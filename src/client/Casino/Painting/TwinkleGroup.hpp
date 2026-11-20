#pragma once

#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client {

/**
 * @brief Points of light that dim together, a star or a crystal each: their
 * pixels, already in the color they dim to, and their beat, the sine of
 * phase plus speed radians a second; they dim while that sine is above
 * dimAbove.
 */
struct TwinkleGroup {
  std::vector<ColoredPixel> pixels;
  float phase{};
  float speed{};
  float dimAbove{};
};

}  // namespace rtype::client
