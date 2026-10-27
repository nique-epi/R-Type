#include "InterfacePixel.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "BackgroundConstants.hpp"

namespace rtype::client {

sf::Vector2i interfacePixelAt(sf::Vector2f playfieldPoint) {
  return {
      static_cast<int>(std::floor(playfieldPoint.x / BACKGROUND_PIXEL_SIZE)),
      static_cast<int>(std::floor(playfieldPoint.y / BACKGROUND_PIXEL_SIZE))};
}

}  // namespace rtype::client
