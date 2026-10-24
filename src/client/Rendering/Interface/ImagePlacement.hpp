#pragma once

#include <SFML/System/Vector2.hpp>
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief Where IInterfaceCanvas::drawImage() lays a copy, and how many times
 * it enlarges it. */
struct ImagePlacement {
  /** @brief Where the top-left pixel of the copied rectangle lands. */
  sf::Vector2i topLeft;
  /** @brief Whole factor each pixel of the texture is enlarged by,
   * NATIVE_SCALE for a copy pixel for pixel. */
  int scale = NATIVE_SCALE;
};

}  // namespace rtype::client
