#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "RowWindow.hpp"

namespace rtype::client::testing {

/** @brief Cells of the locker by their place: three to a row, thirty items in
 * ten rows, four shown at a time. */
constexpr std::size_t FIRST_CELL = 0;
constexpr std::size_t SECOND_CELL = 1;
constexpr std::size_t CROWN_CELL = 2;
constexpr std::size_t MIDDLE_LEFT_CELL = 3;
constexpr std::size_t MIDDLE_CELL = 4;
constexpr std::size_t MIDDLE_RIGHT_CELL = 5;
constexpr std::size_t EIGHTH_CELL = 7;
/** @brief The first cell of the fourth row, the last one shown at first, and
 * the cell under it, in the fifth row; the last item, and the one above. */
constexpr std::size_t FOURTH_ROW_CELL = 9;
constexpr std::size_t FIFTH_ROW_CELL = 12;
constexpr std::size_t ABOVE_THE_LAST_ITEM = 26;
constexpr std::size_t LAST_ITEM = 29;
constexpr std::size_t ROW_OF_THE_LAST_ITEM = 9;

/** @brief The locker showing its rows from the first, and from the third:
 * the cell of the crown then holds item 2 x 3 + 2. */
constexpr std::size_t FROM_THE_FIRST_ROW = 0;
constexpr std::size_t FROM_THE_THIRD_ROW = 2;
constexpr std::size_t UNDER_THE_CROWN_CELL_FROM_THE_THIRD_ROW = 8;
/** @brief The last four rows: the first item is then kept in its column in
 * row 6, item 18; from the top, the last item is kept in row 3, item 11. */
constexpr RowWindow LAST_ROWS{.first = 6, .shown = 4, .count = 10};
constexpr RowWindow FIRST_ROWS{.first = 0, .shown = 4, .count = 10};
constexpr std::size_t FIRST_COLUMN_OF_THE_SEVENTH_ROW = 18;
constexpr std::size_t LAST_COLUMN_OF_THE_FOURTH_ROW = 11;

/** @brief The middle of the cell of the crown, the third of the first row:
 * x 24 + 10 + 2 x 41 + 18 = 134, y 34 + 18 + 18 = 70. */
constexpr sf::Vector2i CROWN_CELL_MIDDLE{134, 70};
/** @brief Between the first two cells: the first ends at x 24 + 10 + 36 = 70
 * (excluded), the second starts at 75. */
constexpr sf::Vector2i BETWEEN_TWO_CELLS{72, 60};
/** @brief Under the last row shown, which ends at y 34 + 18 + 159 = 211. */
constexpr sf::Vector2i UNDER_THE_CELLS{93, 215};

}  // namespace rtype::client::testing
