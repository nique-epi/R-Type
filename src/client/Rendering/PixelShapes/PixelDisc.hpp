#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A filled disc: radius 0 is one pixel, a negative radius nothing. */
struct PixelDisc {
  sf::Vector2i center;
  int radius{};
  sf::Color color;
};

}  // namespace rtype::client
