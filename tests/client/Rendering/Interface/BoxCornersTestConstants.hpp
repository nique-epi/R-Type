#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace rtype::client::testing {

/** @brief A box 30 pixels wide and 40 tall at (10, 20), a closed box, no
 * row tall, as the squadron's box is when its corners meet, and the color
 * their corners are painted in. */
constexpr sf::IntRect CORNERED_BOX({10, 20}, {30, 40});
constexpr sf::IntRect CLOSED_BOX({10, 20}, {30, 0});
constexpr sf::Color CORNER_TEST_COLOR(1, 2, 3);

/** @brief The eight strokes of the corners of CORNERED_BOX, 8 pixels long:
 * along its top row (20) and its bottom row (59), its left column (10) and
 * its right column (39); the strokes of the right end start 8 pixels before
 * it (32), those of the bottom 8 rows above it (52). */
constexpr sf::IntRect TOP_LEFT_ACROSS({10, 20}, {8, 1});
constexpr sf::IntRect TOP_LEFT_DOWN({10, 20}, {1, 8});
constexpr sf::IntRect TOP_RIGHT_ACROSS({32, 20}, {8, 1});
constexpr sf::IntRect TOP_RIGHT_DOWN({39, 20}, {1, 8});
constexpr sf::IntRect BOTTOM_LEFT_ACROSS({10, 59}, {8, 1});
constexpr sf::IntRect BOTTOM_LEFT_UP({10, 52}, {1, 8});
constexpr sf::IntRect BOTTOM_RIGHT_ACROSS({32, 59}, {8, 1});
constexpr sf::IntRect BOTTOM_RIGHT_UP({39, 52}, {1, 8});

}  // namespace rtype::client::testing
