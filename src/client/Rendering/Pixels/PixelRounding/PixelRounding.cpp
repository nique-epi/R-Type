#include "PixelRounding.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace rtype::client {

sf::Vector2i nearestPixel(sf::Vector2f position) {
  return {static_cast<int>(std::lround(position.x)),
          static_cast<int>(std::lround(position.y))};
}

sf::Vector2i pixelAlong(sf::Vector2i start, sf::Vector2i end, float share) {
  return start + nearestPixel(sf::Vector2f(end - start) * share);
}

float shiftPastPixel(float position) {
  const float shift = position - std::floor(position);
  return shift < 1.0F ? shift : 0.0F;
}

int enlargedPixels(float backgroundPixels, unsigned int factor) {
  return static_cast<int>(
      std::lround(backgroundPixels * static_cast<float>(factor)));
}

}  // namespace rtype::client
