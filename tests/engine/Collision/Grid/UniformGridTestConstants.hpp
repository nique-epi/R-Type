#pragma once

#include <cstddef>
#include <limits>
#include "Bounds.hpp"

/** @brief A grid of 4 columns by 2 rows of cells of side 100, its top-left
 * corner on the origin: x 0 to 400, y 0 to 200. */
constexpr float TEST_CELL_SIDE = 100.0F;
constexpr rtype::engine::Bounds TEST_AREA{
    .centerX = 200.0F, .centerY = 100.0F, .width = 400.0F, .height = 200.0F};

/** @brief An area with no size: a grid over it has a single cell. */
constexpr rtype::engine::Bounds AREA_WITHOUT_SIZE{
    .centerX = 50.0F, .centerY = 50.0F, .width = 0.0F, .height = 0.0F};

/** @brief A cell side the grid refuses, being negative. */
constexpr float NEGATIVE_CELL_SIDE = -100.0F;

/** @brief A cell side that would lay 4,000 columns over TEST_AREA, past the
 * most a grid may hold. */
constexpr float TINY_CELL_SIDE = 0.1F;

/** @brief Side of a small square, which fits in one cell of TEST_AREA. */
constexpr float SMALL_SQUARE_SIDE_IN_GRID = 10.0F;

/** @brief Two small squares in the cell of column 0 and row 0. */
constexpr rtype::engine::Bounds FIRST_CELL_SQUARE{
    .centerX = 50.0F,
    .centerY = 50.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};
constexpr rtype::engine::Bounds OTHER_FIRST_CELL_SQUARE{
    .centerX = 60.0F,
    .centerY = 50.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief A small square in the last cell, column 3 and row 1. */
constexpr rtype::engine::Bounds LAST_CELL_SQUARE{
    .centerX = 350.0F,
    .centerY = 150.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief A box from x 50 to 250 and y 50 to 150: columns 0 to 2 of rows 0
 * and 1, six cells. */
constexpr rtype::engine::Bounds WIDE_BOX{
    .centerX = 150.0F, .centerY = 100.0F, .width = 200.0F, .height = 100.0F};

/** @brief A box from x 120 to 280 and y 50 to 150: columns 1 and 2 of rows 0
 * and 1, four cells WIDE_BOX covers too. */
constexpr rtype::engine::Bounds SECOND_WIDE_BOX{
    .centerX = 200.0F, .centerY = 100.0F, .width = 160.0F, .height = 100.0F};

/** @brief Small squares in two more cells WIDE_BOX covers, column 1 of row 1
 * and column 2 of row 0, besides FIRST_CELL_SQUARE in column 0 of row 0. */
constexpr rtype::engine::Bounds SECOND_ROW_SQUARE{
    .centerX = 150.0F,
    .centerY = 150.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};
constexpr rtype::engine::Bounds THIRD_COLUMN_SQUARE{
    .centerX = 230.0F,
    .centerY = 50.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief Two small squares on row 0, left of TEST_AREA: one 50 units beyond
 * its edge, one 5,000 units beyond. */
constexpr rtype::engine::Bounds BEYOND_LEFT_EDGE_SQUARE{
    .centerX = -50.0F,
    .centerY = 50.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};
constexpr rtype::engine::Bounds FAR_BEYOND_LEFT_EDGE_SQUARE{
    .centerX = -5000.0F,
    .centerY = 50.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief A small square on row 1, a billion units right of TEST_AREA. */
constexpr rtype::engine::Bounds FAR_BEYOND_RIGHT_EDGE_SQUARE{
    .centerX = 1.0e9F,
    .centerY = 150.0F,
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief A small square whose center is not a number. */
constexpr rtype::engine::Bounds SQUARE_WITHOUT_CENTER{
    .centerX = std::numeric_limits<float>::quiet_NaN(),
    .centerY = std::numeric_limits<float>::quiet_NaN(),
    .width = SMALL_SQUARE_SIDE_IN_GRID,
    .height = SMALL_SQUARE_SIDE_IN_GRID};

/** @brief The numbers of the boxes, in the order a test inserts them. */
constexpr std::size_t FIRST_BOX = 0;
constexpr std::size_t SECOND_BOX = 1;
constexpr std::size_t THIRD_BOX = 2;
constexpr std::size_t FOURTH_BOX = 3;
