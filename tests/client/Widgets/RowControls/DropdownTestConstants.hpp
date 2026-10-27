#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>

namespace rtype::client::testing {

/**
 * @brief The list of window sizes on the picture the team chose: its box at
 * (332, 72), 112 by 12; open, the list at (332, 84), 112 by 74, a line of 14
 * pixels per size from (334, 86). On a 1920 x 1080 desktop, the first three
 * sizes fit; the largest of them, 1600 x 900, is the third; 960 x 540 is the
 * first and 1280 x 720 the second.
 */
constexpr std::array WINDOW_SIZE_TEXTS{"960 X 540", "1280 X 720", "1600 X 900",
                                       "1920 X 1080", "2560 X 1440"};
constexpr sf::IntRect WINDOW_SIZE_BOX({332, 72}, {112, 12});
constexpr sf::IntRect OPEN_LIST({332, 84}, {112, 74});
constexpr int FIRST_LINE_MIDDLE = 86 + 7;
constexpr int LINE_HEIGHT = 14;
constexpr std::size_t SIZES_THAT_FIT = 3;
constexpr std::size_t LARGEST_SIZE_INDEX = 2;
constexpr std::size_t FIRST_SIZE_INDEX = 0;
constexpr std::size_t SECOND_SIZE_INDEX = 1;
constexpr std::size_t TOO_LARGE = 4;
constexpr sf::Vector2i OUTSIDE(100, 100);

}  // namespace rtype::client::testing
