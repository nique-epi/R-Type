#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace rtype::client {

/** @brief Where a number of the drawn numbers row is painted, and the color
 * of its edges: gold for the newest, bronze for the others. */
struct LastNumberBox {
  sf::IntRect area;
  sf::Color edge;
};

}  // namespace rtype::client
