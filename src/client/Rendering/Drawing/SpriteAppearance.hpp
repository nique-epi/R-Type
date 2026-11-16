#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "SpriteTone.hpp"

namespace rtype::client {

/** @brief How an entity is drawn beyond its sprite: its tone and outline,
 * how many pixels of the canvas away from its Position, how many rows of
 * its frame show, from the top, with or without a leading line, and how far
 * through its roll it is. */
struct SpriteAppearance {
  SpriteTone tone{};
  std::optional<sf::Color> outline;
  sf::Vector2i offset;
  std::optional<int> shownRows;
  bool leadingLine = false;
  /** @brief How far through its barrel roll the entity is drawn, from 0 to 1
   * (game::rolledRowsOf()); nothing while it does not roll. */
  std::optional<float> rollShare;
};

}  // namespace rtype::client
