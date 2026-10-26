#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief Where IInterfaceCanvas::drawImage() lays a copy, how many times it
 * enlarges it, and the color it multiplies it by. */
struct ImagePlacement {
  /** @brief Where the top-left pixel of the copied rectangle lands. */
  sf::Vector2i topLeft;
  /** @brief Whole factor each pixel of the texture is enlarged by,
   * NATIVE_SCALE for a copy pixel for pixel. */
  int scale = NATIVE_SCALE;
  /** @brief Color each pixel of the copy is multiplied by, channel by
   * channel, alpha included: white leaves the copy as it is, a dark shade
   * darkens it, a translucent one lets what lies under show. */
  sf::Color tint = sf::Color::White;
};

}  // namespace rtype::client
