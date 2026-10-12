#include "PixelRounding.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace rtype::client {

sf::Vector2i nearestPixel(sf::Vector2f position) {
  return {static_cast<int>(std::lround(position.x)),
          static_cast<int>(std::lround(position.y))};
}

}  // namespace rtype::client
