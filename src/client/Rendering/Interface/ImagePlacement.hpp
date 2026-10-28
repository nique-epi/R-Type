#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "InterfaceConstants.hpp"
#include "ShadeSwap.hpp"

namespace rtype::client {

/** @brief Where IInterfaceCanvas::drawImage() lays a copy, how many times it
 * enlarges it, the color it multiplies it by, and the shades it swaps. */
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
  /** @brief The shades of a player color the copy shows in those of the
   * palette the player chose, before the tint; none keeps the colors of the
   * texture. Where the graphics card has no shaders, the copy keeps them and
   * the log of the canvas says so once. */
  std::optional<ShadeSwap> recolor = std::nullopt;
};

}  // namespace rtype::client
