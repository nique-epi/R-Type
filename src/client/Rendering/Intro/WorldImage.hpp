#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "TextureArea.hpp"

namespace rtype::client {

/** @brief An image of a sheet laid on the world: where its top-left corner
 * is, in pixels of the world, the color it is multiplied by, and whether it
 * is debris the boost leaves behind. */
struct WorldImage {
  TextureArea image;
  sf::Vector2f topLeft;
  sf::Color tint = sf::Color::White;
  bool leftBehind{};
};

}  // namespace rtype::client
