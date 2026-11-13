#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "ScreenId.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @brief A place of the whole map that is no server: a glyph a click or a
 * key opens a screen from. */
struct MapPlace {
  /** @brief Pixel of the interface its glyph is centered on. */
  sf::Vector2i point;
  /** @brief Its glyph on the sheet of the map. */
  game::SpriteFrame frame{};
  /** @brief Its name, written beside it and at the top of its bubble. */
  const char* label{};
  /** @brief What its bubble says it holds. */
  const char* detail{};
  /** @brief The key that opens it, and how its bubble names that key. */
  sf::Keyboard::Key key{};
  const char* keyName{};
  /** @brief The screen it pushes over the map. */
  ScreenId screen;
  /** @brief From its point to the top-left corner of its bubble. */
  sf::Vector2i bubbleOffset;
};

}  // namespace rtype::client
