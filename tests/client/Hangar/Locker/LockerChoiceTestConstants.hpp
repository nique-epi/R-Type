#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client::testing {

/** @brief Cells of the locker by their place: three to a row, ten items. */
constexpr std::size_t FIRST_CELL = 0;
constexpr std::size_t SECOND_CELL = 1;
constexpr std::size_t CROWN_CELL = 2;
constexpr std::size_t MIDDLE_LEFT_CELL = 3;
constexpr std::size_t MIDDLE_CELL = 4;
constexpr std::size_t MIDDLE_RIGHT_CELL = 5;
constexpr std::size_t SEVENTH_CELL = 6;
constexpr std::size_t EIGHTH_CELL = 7;
constexpr std::size_t NINTH_CELL = 8;
constexpr std::size_t LAST_ITEM_CELL = 9;

/** @brief The middle of the cell of the crown, the third of the first row:
 * x 24 + 10 + 2 x 41 + 18 = 134, y 34 + 18 + 18 = 70. */
constexpr sf::Vector2i CROWN_CELL_MIDDLE{134, 70};
/** @brief Between the first two cells: the first ends at x 24 + 10 + 36 = 70
 * (excluded), the second starts at 75. */
constexpr sf::Vector2i BETWEEN_TWO_CELLS{72, 60};
/** @brief The middle of cell 10, the second of the fourth row, which holds no
 * item: x 24 + 10 + 41 + 18 = 93, y 34 + 18 + 3 x 41 + 18 = 193. */
constexpr sf::Vector2i EMPTY_CELL_MIDDLE{93, 193};

}  // namespace rtype::client::testing
