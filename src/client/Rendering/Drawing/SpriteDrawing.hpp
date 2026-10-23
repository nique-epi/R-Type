#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <string_view>
#include "Position.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"

namespace rtype::client {

/**
 * @brief One rectangle of an image to draw: which image, which rectangle,
 * where, how its pixels are colored, and the outline drawn around its shape
 * first.
 */
struct SpriteDrawing {
  /** @brief Id of the image. */
  std::string_view assetId;
  /** @brief Rectangle of the image to show, in pixels of the image. */
  game::SpriteFrame frame{};
  /** @brief Center of the rectangle, in logical units of the playfield. */
  game::Position center{};
  SpriteTone tone{};
  /** @brief Color of a one-pixel outline around the visible pixels, or
   * nothing for none. */
  std::optional<sf::Color> outline;
};

}  // namespace rtype::client
