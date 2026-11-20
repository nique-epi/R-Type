#pragma once

#include <array>
#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client {

/**
 * @brief A spectator of the crowd of the hall, cut out of its picture: its
 * pixels raised one, then two pixels, since at rest it is the picture itself,
 * and its beat, the sine of phase plus speed radians a second.
 */
struct Spectator {
  std::array<std::vector<ColoredPixel>, 2> raised;
  float phase{};
  float speed{};
};

}  // namespace rtype::client
