#pragma once

namespace rtype::engine {

/**
 * @brief Axis-aligned rectangle, given by its center and its size.
 *
 * It extends by half its width on each side of centerX and by half its
 * height above and below centerY. The units are the caller's, as long as both
 * rectangles given to overlaps() use the same ones.
 */
struct Bounds {
  float centerX;
  float centerY;
  float width;
  float height;
};

}  // namespace rtype::engine
