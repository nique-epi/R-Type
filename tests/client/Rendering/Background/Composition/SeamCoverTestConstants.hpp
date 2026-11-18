#pragma once

#include <cstddef>

/**
 * @brief Columns counted from the seam, read on sixteen rows, four times the
 * side of the dither matrix: column 8 lies halfway through the edge of 16
 * columns, and the matrix holds two levels of eight and more in its column 0
 * (12 and 15), so half its pixels stay hidden; column 15 is the last of the
 * edge, and the matrix holds no level of fifteen in its column 3, so none
 * does.
 */
constexpr int LEFT_OF_THE_SEAM = -1;
constexpr int FIRST_COLUMN_OF_THE_EDGE = 0;
constexpr int HALFWAY_THROUGH_THE_EDGE = 8;
constexpr int LAST_COLUMN_OF_THE_EDGE = 15;
constexpr int PAST_THE_EDGE = 16;
constexpr int ROWS_READ = 16;
constexpr std::size_t EVERY_ROW = 16;
constexpr std::size_t HALF_THE_ROWS = 8;
constexpr std::size_t NO_ROW = 0;
