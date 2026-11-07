#include "PixelRounding.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace rtype::client {

sf::Vector2i nearestPixel(sf::Vector2f position) {
  return {static_cast<int>(std::lround(position.x)),
          static_cast<int>(std::lround(position.y))};
}

sf::Vector2i pixelAlong(sf::Vector2i from, sf::Vector2i to, float share) {
  return from + nearestPixel(sf::Vector2f(to - from) * share);
}

}  // namespace rtype::client
