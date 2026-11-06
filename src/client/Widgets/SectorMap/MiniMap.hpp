#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace rtype::client {

/**
 * @brief The whole map in small, shown while zoomed in: its frame on the
 * grid, and, in pixels of the map zoomed out, the centre of the planet, a
 * point per server and the part of the map on screen.
 */
struct MiniMap {
  sf::IntRect area;
  sf::Vector2i planetCenter;
  int planetRadius{};
  std::vector<sf::Vector2i> stars;
  sf::IntRect shown;
};

}  // namespace rtype::client
