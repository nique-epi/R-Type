#pragma once

#include "Bounds.hpp"
#include "MaskArea.hpp"
#include "MaskPixel.hpp"

namespace rtype::game::testing {

/** @brief A mask as large as the frame of the ship, 32 by 16 pixels. */
constexpr int FRAME_WIDTH = 32;
constexpr int FRAME_HEIGHT = 16;

/** @brief A center that falls on the edge of a cell of 4 units on each axis:
 * 64 / 4 - 32 / 2 = 0 and 32 / 4 - 16 / 2 = 0, a whole cell. */
constexpr float CENTER_ON_A_CELL_EDGE_X = 64.0F;
constexpr float CENTER_ON_A_CELL_EDGE_Y = 32.0F;
constexpr int CELL_UNDER_A_CENTER_ON_AN_EDGE = 0;

/** @brief Centers half a cell right of and left of that edge, and half a cell
 * below it: 66 / 4 - 16 = 0.5, 62 / 4 - 16 = -0.5 and 34 / 4 - 8 = 0.5, which
 * the client's nearestPixel() rounds away from zero, to 1, -1 and 1. */
constexpr float CENTER_HALF_A_CELL_RIGHT_X = 66.0F;
constexpr float CENTER_HALF_A_CELL_LEFT_X = 62.0F;
constexpr float CENTER_HALF_A_CELL_BELOW_Y = 34.0F;
constexpr int CELL_HALF_A_CELL_RIGHT = 1;
constexpr int CELL_HALF_A_CELL_LEFT = -1;
constexpr int CELL_HALF_A_CELL_BELOW = 1;

/** @brief A box of 4 by 4 units from 8 to 12 and from 0 to 4: its inside
 * covers cell 2 of the columns and cell 0 of the rows, nothing more. */
constexpr engine::Bounds BOX_ON_CELL_EDGES{
    .centerX = 10.0F, .centerY = 2.0F, .width = 4.0F, .height = 4.0F};
constexpr engine::MaskArea CELL_OF_THE_BOX_ON_EDGES{
    .left = 2, .top = 0, .width = 1, .height = 1};

/** @brief The same box moved 0.1 unit left, from 7.9 to 11.9: its inside
 * reaches into column 1 too. */
constexpr engine::Bounds BOX_A_TENTH_LEFT{
    .centerX = 9.9F, .centerY = 2.0F, .width = 4.0F, .height = 4.0F};
constexpr engine::MaskArea CELLS_OF_THE_BOX_A_TENTH_LEFT{
    .left = 1, .top = 0, .width = 2, .height = 1};

/** @brief A box of no width standing on the edge between columns 1 and 2: its
 * inside covers no column. */
constexpr engine::Bounds BOX_OF_NO_WIDTH{
    .centerX = 8.0F, .centerY = 2.0F, .width = 0.0F, .height = 4.0F};
constexpr int NO_CELL = 0;

/** @brief A mask of two pixels side by side, the left one alone drawn, laid
 * at cell 0, 0; the box of its body, 8 by 4 units over those two cells. */
constexpr int TWO_PIXELS_WIDE = 2;
constexpr int ONE_PIXEL_TALL = 1;
constexpr engine::MaskPixel LEFT_PIXEL{.x = 0, .y = 0};
constexpr engine::Bounds BODY_OVER_TWO_CELLS{
    .centerX = 4.0F, .centerY = 2.0F, .width = 8.0F, .height = 4.0F};

/** @brief The same mask laid one cell right, at cell 1, 0, and its box. */
constexpr int ONE_CELL_RIGHT = 1;
constexpr engine::Bounds BODY_ONE_CELL_RIGHT{
    .centerX = 8.0F, .centerY = 2.0F, .width = 8.0F, .height = 4.0F};

/** @brief A box of 2 by 2 units inside cell 0 (from 1 to 3), over the drawn
 * pixel, and one inside cell 1 (from 5 to 7), over the clear pixel. */
constexpr engine::Bounds BOX_IN_THE_DRAWN_CELL{
    .centerX = 2.0F, .centerY = 2.0F, .width = 2.0F, .height = 2.0F};
constexpr engine::Bounds BOX_IN_THE_CLEAR_CELL{
    .centerX = 6.0F, .centerY = 2.0F, .width = 2.0F, .height = 2.0F};

/** @brief A box far from every other box of these tests. */
constexpr engine::Bounds FAR_AWAY_BOX{
    .centerX = 500.0F, .centerY = 500.0F, .width = 2.0F, .height = 2.0F};

}  // namespace rtype::game::testing
