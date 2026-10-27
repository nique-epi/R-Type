#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "SpriteTone.hpp"

namespace rtype::client {

/** @brief How an entity is drawn beyond its sprite: its tone and outline,
 * how many pixels of the canvas away from its Position, how many rows of
 * its frame show, from the top, with or without a leading line, and the
 * angle of its roll. */
struct SpriteAppearance {
  SpriteTone tone{};
  std::optional<sf::Color> outline;
  sf::Vector2i offset;
  std::optional<int> shownRows;
  bool leadingLine = false;
  /** @brief The angle of the barrel roll drawn, in radians; nothing while the
   * entity does not roll. */
  std::optional<float> rollAngle;
};

}  // namespace rtype::client
