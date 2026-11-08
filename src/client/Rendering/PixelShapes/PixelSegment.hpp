#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A straight stroke from one pixel to another, both drawn, thickness
 * pixels wide. */
struct PixelSegment {
  sf::Vector2i from;
  sf::Vector2i to;
  int thickness = 1;
  sf::Color color;
};

}  // namespace rtype::client
