#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include "RowWindow.hpp"

namespace rtype::client::testing {

/** @brief Ten rows shown four at a time, from the first, from the second and
 * from the last row a window starts from, the seventh; a grid of three rows,
 * which all show. */
constexpr RowWindow FROM_THE_TOP{.first = 0, .shown = 4, .count = 10};
constexpr RowWindow FROM_THE_SECOND_ROW{.first = 1, .shown = 4, .count = 10};
constexpr RowWindow FROM_THE_LAST_START{.first = 6, .shown = 4, .count = 10};
constexpr RowWindow ALL_SHOWN{.first = 0, .shown = 4, .count = 3};
constexpr std::size_t FIRST_ROW = 0;
constexpr std::size_t FOURTH_ROW = 3;
constexpr std::size_t FIFTH_ROW = 4;
constexpr std::size_t LAST_ROW = 9;
constexpr int ONE_ROW_DOWN = 1;
constexpr int ONE_ROW_UP = -1;
/** @brief A track of 159 rows at (158, 52), 2 wide: its thumb is 159 x 4 /
 * 10 = 63 long, at the top, then 96 / 6 = 16 lower per row. */
constexpr sf::IntRect TEN_ROW_TRACK({158, 52}, {2, 159});
constexpr sf::IntRect THUMB_AT_THE_TOP({158, 52}, {2, 63});
constexpr sf::IntRect THUMB_ONE_ROW_DOWN({158, 68}, {2, 63});
constexpr sf::IntRect THUMB_AT_THE_BOTTOM({158, 148}, {2, 63});

}  // namespace rtype::client::testing
