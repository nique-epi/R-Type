#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief An amount of kopecks: gold digits, the coin KOPECKS_COIN_GAP left
 * of them, as tall as the coin; a gain starts with KOPECKS_GAIN_PREFIX. */
constexpr sf::Color KOPECKS_COLOR = STAR_GOLD;
constexpr int KOPECKS_COIN_GAP = 3;
constexpr int KOPECKS_LABEL_HEIGHT = KOPECK_FRAME.height;
constexpr std::string_view KOPECKS_GAIN_PREFIX = "+";

/**
 * @brief The balance of the player, in the top-right corner of the safe
 * margin: a cut-corner frame as wide as the largest balance needs
 * (KOPECKS_WIDEST_AMOUNT_LETTERS letters, "999 999"), the amount
 * KOPECKS_FRAME_PADDING inside it, and KOPECKS_CAPTION in small print
 * KOPECKS_CAPTION_GAP under the amount, both ending on the same column.
 */
constexpr sf::Vector2i KOPECKS_BALANCE_TOP_RIGHT(INTERFACE_AREA.size.x -
                                                     SAFE_MARGIN,
                                                 SAFE_MARGIN);
constexpr int KOPECKS_WIDEST_AMOUNT_LETTERS = 7;
constexpr sf::Vector2i KOPECKS_FRAME_PADDING(8, 5);
constexpr int KOPECKS_CAPTION_GAP = 2;
constexpr sf::Vector2i KOPECKS_FRAME_SIZE(
    (SIDES_PER_AXIS * KOPECKS_FRAME_PADDING.x) + KOPECK_FRAME.width +
        KOPECKS_COIN_GAP +
        (KOPECKS_WIDEST_AMOUNT_LETTERS * REGULAR_FONT_ADVANCE),
    (SIDES_PER_AXIS * KOPECKS_FRAME_PADDING.y) + KOPECKS_LABEL_HEIGHT +
        KOPECKS_CAPTION_GAP + TEXT_LINE_HEIGHT);
constexpr const char* KOPECKS_CAPTION = "KOPECKS";
constexpr sf::Color KOPECKS_FRAME_BORDER_COLOR = EDGE_BLUE;
constexpr sf::Color KOPECKS_FRAME_INSIDE_COLOR = SKY_COLOR;
constexpr sf::Color KOPECKS_CAPTION_COLOR = PALE_BLUE;

}  // namespace rtype::client
