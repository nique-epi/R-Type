#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client::testing {

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
