#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "ShadeSwap.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

/** @brief An image of a sheet laid on the world: where its top-left corner
 * is, in pixels of the world, the color it is multiplied by, whether it is
 * debris the boost leaves behind, and the shades of a pilot it swaps. */
struct WorldImage {
  TextureArea image;
  sf::Vector2f topLeft;
  sf::Color tint = sf::Color::White;
  bool leftBehind{};
  std::optional<ShadeSwap> recolor = std::nullopt;
};

}  // namespace rtype::client
