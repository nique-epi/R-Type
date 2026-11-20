#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/**
 * @brief The golds of the casino, from the darkest: the inner edge of a
 * panel, its outer edge, the inner corners, and the outer corners and the
 * names on the tags.
 */
constexpr sf::Color CASINO_DARK_GOLD(92, 52, 20);
constexpr sf::Color CASINO_GOLD(196, 135, 31);
constexpr sf::Color CASINO_LIGHT_GOLD(234, 181, 60);
constexpr sf::Color CASINO_TAG_GOLD = STAR_GOLD;

/**
 * @brief The lights of the Bydo Palace: a bulb out, brick red; the warm cream
 * of a glow; the plum of the neon sign when it cuts out; the deep navy of the
 * night sky; the red of a refusal and of the camera light; the cream of a lit
 * rate of the exchange board; and white, the flash of a camera.
 */
constexpr sf::Color CASINO_BRICK_RED(134, 56, 52);
constexpr sf::Color CASINO_WARM_CREAM(251, 228, 184);
constexpr sf::Color CASINO_PLUM(58, 12, 48);
constexpr sf::Color CASINO_DEEP_NAVY(13, 10, 35);
constexpr sf::Color CASINO_NEON_RED(198, 36, 33);
constexpr sf::Color CASINO_PALE_CREAM(232, 220, 181);
constexpr sf::Color CASINO_FLASH_WHITE(255, 255, 255);

/**
 * @brief A panel of the casino: the panel of the client, edged twice in gold,
 * the inner edge CASINO_PANEL_INNER_EDGE inside, with the cut corners of a
 * gaming plaque: an outer corner of one pixel, an inner corner of
 * CASINO_PANEL_CORNER_LENGTH pixels along each side.
 */
constexpr sf::Color CASINO_PANEL_INSIDE_COLOR = SKY_COLOR;
constexpr int CASINO_PANEL_INNER_EDGE = 2;
constexpr int CASINO_PANEL_OUTER_CORNER_LENGTH = 1;
constexpr int CASINO_PANEL_CORNER_LENGTH = 2;

/**
 * @brief The tag that names a panel, astride its top edge: CASINO_TAG_HEIGHT
 * tall, its name CASINO_TAG_PADDING in from its left side and its top, and
 * that much free on the right of the name.
 */
constexpr int CASINO_TAG_HEIGHT = 12;
constexpr sf::Vector2i CASINO_TAG_PADDING(6, 2);
constexpr TextStyle CASINO_TAG_STYLE{.font = TextFont::Regular,
                                     .color = CASINO_TAG_GOLD};

/**
 * @brief The two balances of the casino in the top-right corner: a frame of
 * the client, the kopecks in gold and the chips in white, each over its name
 * in small print, CASINO_BALANCES_PADDING in from the right side.
 */
constexpr sf::IntRect CASINO_BALANCES_AREA({374, 8}, {96, 42});
constexpr int CASINO_BALANCES_PADDING = 6;
constexpr int CASINO_KOPECKS_TOP = 4;
constexpr int CASINO_KOPECKS_CAPTION_TOP = 12;
constexpr int CASINO_CHIPS_TOP = 23;
constexpr int CASINO_CHIPS_CAPTION_TOP = 31;
constexpr const char* CASINO_KOPECKS_CAPTION = "KOPECKS";
constexpr const char* CASINO_CHIPS_CAPTION = "CHIPS";
constexpr sf::Color CASINO_BALANCES_BORDER_COLOR = EDGE_BLUE;
constexpr TextStyle CASINO_KOPECKS_STYLE{.font = TextFont::Regular,
                                         .color = STAR_GOLD,
                                         .alignment = TextAlignment::Right};
constexpr TextStyle CASINO_CHIPS_STYLE{.font = TextFont::Regular,
                                       .color = STAR_WHITE,
                                       .alignment = TextAlignment::Right};
constexpr TextStyle CASINO_CAPTION_STYLE{.font = TextFont::Small,
                                         .color = PALE_BLUE,
                                         .alignment = TextAlignment::Right};

/**
 * @brief What the butler says, in a panel across the bottom of the screen:
 * his name on its tag, CASINO_SPEECH_TAG_LEFT in from its left side, and two
 * lines CASINO_SPEECH_LINE_GAP apart, the first at CASINO_SPEECH_FIRST_LINE
 * from its top-left corner.
 */
constexpr sf::IntRect CASINO_SPEECH_AREA({60, 222}, {360, 40});
constexpr int CASINO_SPEECH_TAG_LEFT = 10;
constexpr sf::Vector2i CASINO_SPEECH_FIRST_LINE(12, 10);
constexpr int CASINO_SPEECH_LINE_GAP = 11;
constexpr const char* BUTLER_NAME = "BALTHAZAR";
constexpr TextStyle CASINO_SPEECH_STYLE{.font = TextFont::Regular,
                                        .color = STAR_WHITE};

}  // namespace rtype::client
