#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief A word on ink: how tall the ink is, the room the word leaves on it,
 * where the word stands on it, and the color of the ink. */
constexpr int INK_TAG_HEIGHT = 9;
constexpr int INK_TAG_PADDING = 5;
constexpr sf::Vector2i INK_TAG_TEXT_OFFSET(3, 1);
constexpr sf::Color INK_TAG_COLOR = INK;

}  // namespace rtype::client
