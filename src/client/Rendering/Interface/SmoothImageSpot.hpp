#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace rtype::client {

/** @brief Where a smooth image is laid, in pixels of the interface, and the
 * color it is tinted with. */
struct SmoothImageSpot {
  sf::IntRect area;
  sf::Color tint;
};

}  // namespace rtype::client
