#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief A plane of the starfield, from the farthest to the nearest. Each one
 * is painted apart, so a slow plane can move by less than a pixel.
 */
enum class StarPlane : std::uint8_t {
  FarStars,
  Planet,
  MiddleStars,
  NearStars
};

}  // namespace rtype::client
