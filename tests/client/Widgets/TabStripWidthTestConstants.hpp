#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client::testing {

/** @brief The strip of the leaderboard on the picture the team chose: 320 wide
 * at (80, 50). Its two tabs share what the keycaps leave: 140 each, at 98 and
 * 242, and the keycap of the next tab starts at 386. */
constexpr int NARROW_WIDTH = 320;
constexpr sf::Vector2i NARROW_TOP_LEFT(80, 50);
constexpr sf::Vector2i IN_THE_SECOND_TAB(242, 55);
constexpr sf::Vector2i LEFT_OF_THE_SECOND_TAB(241, 55);
constexpr sf::Vector2i ON_THE_NEXT_KEYCAP(386, 55);
constexpr sf::Vector2i RIGHT_OF_THE_STRIP(400, 55);
constexpr std::size_t SECOND_TAB = 1;

}  // namespace rtype::client::testing
