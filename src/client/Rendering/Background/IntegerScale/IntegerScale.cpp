#include "IntegerScale.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "BackgroundConstants.hpp"

namespace rtype::client {

unsigned int integerScaleFactor(sf::Vector2i playfieldPixels) {
  const int horizontal =
      playfieldPixels.x / static_cast<int>(BACKGROUND_SIZE.x);
  const int vertical = playfieldPixels.y / static_cast<int>(BACKGROUND_SIZE.y);
  return static_cast<unsigned int>(std::max(1, std::min(horizontal, vertical)));
}

float sharpScaleOf(unsigned int factor, unsigned int density) {
  return static_cast<float>(factor) / static_cast<float>(density);
}

}  // namespace rtype::client
