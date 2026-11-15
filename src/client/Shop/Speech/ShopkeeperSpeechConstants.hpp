#pragma once

#include <SFML/System/Vector2.hpp>
#include "ShopPaintingConstants.hpp"
#include "SpokenLineConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The bubble of the shopkeeper: SHOPKEEPER_BUBBLE_WIDTH wide, over
 * the middle of his frame, between the sign of the booth and his head, its
 * tail ending on the row above his frame; a line he says holds at most
 * SHOPKEEPER_MAX_LINES lines there. */
constexpr int SHOPKEEPER_BUBBLE_WIDTH = 210;
constexpr int SHOPKEEPER_MAX_LINES = 3;
constexpr int SHOPKEEPER_MIDDLE_X =
    SHOPKEEPER_TOP_LEFT.x + (SHOPKEEPER_FRAME_SIZE.x / SIDES_PER_AXIS);
constexpr sf::Vector2i SHOPKEEPER_BUBBLE_BOTTOM_LEFT(
    SHOPKEEPER_MIDDLE_X - (SHOPKEEPER_BUBBLE_WIDTH / SIDES_PER_AXIS),
    SHOPKEEPER_TOP_LEFT.y - SPOKEN_TAIL_ROWS);
constexpr int SHOPKEEPER_BUBBLE_TAIL = SHOPKEEPER_BUBBLE_WIDTH / SIDES_PER_AXIS;

}  // namespace rtype::client
