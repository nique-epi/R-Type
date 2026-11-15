#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "TextMetrics.hpp"

namespace rtype::client::testing {

/** @brief The largest text size, and the order of ACCESS among the five tabs
 * of the options. At that size the keycaps are 22 wide, so the tabs share
 * 432 - 2 x 26 = 380 pixels; each is as wide as its label and 4 on each
 * side: SOUND 88, CONTROLS 136, DISPLAY 120, ACCESS 104, CHAT 72. SOUND to
 * ACCESS take 460, CONTROLS to ACCESS 368: those three show, CHAT does not
 * fit after them. */
constexpr TextMetrics LARGEST_TEXT{.halfPixelsPerFontPixel = 4};
constexpr std::size_t ACCESS_OF_FIVE = 3;

/** @brief The tabs of the options on the picture the team chose: four of 96
 * pixels at 42, 142, 242 and 342, the strip at (24, 50), with the keycap of
 * the previous tab on its left. */
constexpr sf::Vector2i STRIP_TOP_LEFT(24, 50);
constexpr int TAB_TOP_ROW = 55;
constexpr sf::Vector2i IN_THE_SECOND_OPTIONS_TAB(150, TAB_TOP_ROW);
constexpr sf::Vector2i BETWEEN_THE_FIRST_OPTIONS_TABS(140, TAB_TOP_ROW);
constexpr sf::Vector2i ON_THE_PREVIOUS_KEYCAP(30, TAB_TOP_ROW);
constexpr std::size_t CONTROLS = 1;
constexpr std::size_t ACCESS = 3;
/** @brief The first row under the border of the panel, at row 66. */
constexpr int UNDER_THE_PANEL_BORDER = 67;

/** @brief The strip of the leaderboard on the picture the team chose: 320 wide
 * at (80, 50). Its two tabs share what the keycaps leave: 140 each, at 98 and
 * 242, and the keycap of the next tab starts at 386. */
constexpr int NARROW_WIDTH = 320;
constexpr sf::Vector2i NARROW_TOP_LEFT(80, 50);
constexpr sf::Vector2i IN_THE_SECOND_TAB(242, TAB_TOP_ROW);
constexpr sf::Vector2i LEFT_OF_THE_SECOND_TAB(241, TAB_TOP_ROW);
constexpr sf::Vector2i ON_THE_NEXT_KEYCAP(386, TAB_TOP_ROW);
constexpr sf::Vector2i RIGHT_OF_THE_STRIP(400, TAB_TOP_ROW);
constexpr std::size_t SECOND_TAB = 1;

}  // namespace rtype::client::testing
