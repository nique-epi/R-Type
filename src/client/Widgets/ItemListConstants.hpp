#pragma once

#include <SFML/Graphics/Color.hpp>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief The highlight of a row: how far it passes the sides of the row and
 * rises above its text, and how tall it is. */
constexpr int ITEM_ROW_MARGIN = 4;
constexpr int ITEM_ROW_SPACE_ABOVE = 2;
constexpr int ITEM_ROW_HEIGHT = 12;

/** @brief The pinned row: its dashed line above the row after the last one, and
 * how far under that place the pinned row starts. */
constexpr int ITEM_PIN_LINE_RISE = 2;
constexpr int ITEM_PINNED_ROW_DROP = 2;
constexpr sf::Color ITEM_PIN_LINE_COLOR = DEEP_BLUE;

/** @brief A tag: how far after its cell it starts, how far it rises above the
 * row, its height, its ink, and the room its word leaves on the ink. */
constexpr int ITEM_TAG_GAP = 3;
constexpr int ITEM_TAG_RISE = 1;
constexpr int ITEM_TAG_HEIGHT = 9;
constexpr int ITEM_TAG_PADDING = 5;
constexpr int ITEM_TAG_TEXT_LEFT = 3;
constexpr sf::Color ITEM_TAG_COLOR = INK;

}  // namespace rtype::client
