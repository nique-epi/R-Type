#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief The highlight of a row: how far it passes the sides of the row and
 * rises above its text, and how tall it is. */
constexpr int ITEM_ROW_MARGIN = 4;
constexpr int ITEM_ROW_SPACE_ABOVE = 2;
constexpr int ITEM_ROW_HEIGHT = 12;

/** @brief The pinned row or note: its dashed line above the row after the last
 * one, how far under that place it starts, and the color of a note. */
constexpr int ITEM_PIN_LINE_RISE = 2;
constexpr int ITEM_PINNED_ROW_DROP = 2;
constexpr sf::Color ITEM_PIN_LINE_COLOR = DEEP_BLUE;
constexpr sf::Color ITEM_NOTE_COLOR = PALE_BLUE;

/** @brief A tag: how far after its cell it starts, how far it rises above the
 * row, its height, its ink, the room its word leaves on the ink, and where its
 * word stands on it, as YOU on the ink of the flight log. */
constexpr int ITEM_TAG_GAP = 3;
constexpr int ITEM_TAG_RISE = 1;
constexpr int ITEM_TAG_HEIGHT = 9;
constexpr int ITEM_TAG_PADDING = 5;
constexpr sf::Vector2i ITEM_TAG_TEXT_OFFSET(3, 1);
constexpr sf::Color ITEM_TAG_COLOR = INK;

}  // namespace rtype::client
