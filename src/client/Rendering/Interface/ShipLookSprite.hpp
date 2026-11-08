#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>

namespace rtype::client {

/** @brief One item of the locker as the sheet draws it. */
struct ShipLookSprite {
  /** @brief The name the locker and the hangar show. */
  std::string_view label;
  /** @brief Size of the drawing, from the top-left corner of its cell. */
  sf::Vector2i size;
  /** @brief Where the top-left corner of the drawing goes, in pixels of the
   * ship frame (32 x 16, nose to the right). */
  sf::Vector2i mount;
};

}  // namespace rtype::client
