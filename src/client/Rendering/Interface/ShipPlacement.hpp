#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief Where a ship frame is drawn on the interface picture: its top-left
 * corner, how many pixels of the picture one pixel of the frame covers, and
 * the area outside which nothing of it, nor of what it wears, is painted. */
struct ShipPlacement {
  sf::Vector2i topLeft;
  int scale = NATIVE_SCALE;
  sf::IntRect visibleArea = INTERFACE_AREA;
};

}  // namespace rtype::client
