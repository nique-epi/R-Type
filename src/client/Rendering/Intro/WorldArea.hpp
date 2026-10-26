#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A rectangle of the world painted in one color: its top-left corner
 * and its size, in pixels of the world; @c leftBehind for debris, which the
 * boost leaves behind. */
struct WorldArea {
  sf::Vector2f topLeft;
  sf::Vector2i size;
  sf::Color color;
  bool leftBehind{};
};

}  // namespace rtype::client
