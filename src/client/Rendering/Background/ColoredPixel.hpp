#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A pixel and the color it is painted with. */
struct ColoredPixel {
  sf::Vector2i position;
  sf::Color color;
};

}  // namespace rtype::client
