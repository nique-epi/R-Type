#pragma once

#include <array>
#include <chrono>
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The side of the diagonal mask of the tests, as tall as the ship. */
constexpr int DIAGONAL_SIDE = 16;

/**
 * @brief A fifth into a roll, cos(0.4 pi) = 0.309: round(16 x 0.309) = 5 rows
 * on the back, copying rows ((2r + 1) x 16) / 10 of the upright mask.
 */
constexpr float A_FIFTH_INTO_THE_ROLL = 0.2F;
constexpr std::array<int, 5> ROWS_SHOWN_A_FIFTH_IN{1, 4, 8, 11, 14};

/**
 * @brief Two fifths into a roll, cos(0.8 pi) = -0.809: round(16 x 0.809) = 13
 * rows on the belly, the first copying row 15 - (16 / 26) = 15 of the upright
 * mask and the last row 15 - (25 x 16 / 26) = 0.
 */
constexpr float TWO_FIFTHS_INTO_THE_ROLL = 0.4F;
constexpr int ROWS_SHOWN_TWO_FIFTHS_IN = 13;
constexpr int FIRST_ROW_SHOWN_TWO_FIFTHS_IN = 15;
constexpr int LAST_ROW_SHOWN_TWO_FIFTHS_IN = 0;

/** @brief The start of a roll, and the last row of the diagonal mask. */
constexpr float START_OF_THE_ROLL = 0.0F;
constexpr int LAST_DIAGONAL_ROW = 15;

/** @brief A mask of one row, shorter than the 2 rows a rolling ship keeps. */
constexpr int ONE_ROW = 1;

/**
 * @brief A ship a quarter turn into its roll of 400 ms, seen edge on: 2 rows,
 * which the client draws centered on its Position, each rounded on its own.
 * At y 34 the first row is centered on 34 - 2 = 32 and drawn on row
 * lround(32 / 4 - 0.5) = 8 of the grid: the rolled mask lies on rows 8 and 9.
 */
constexpr engine::Duration EDGE_ON_INTO_THE_ROLL =
    std::chrono::milliseconds(100);
constexpr Position SHIP_HALF_A_PIXEL_DOWN{.x = 64.0F, .y = 34.0F};
constexpr int ROWS_SEEN_EDGE_ON = 2;
constexpr int FIRST_GRID_ROW_EDGE_ON = 8;

}  // namespace rtype::game::testing
