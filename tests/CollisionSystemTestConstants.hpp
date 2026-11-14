#pragma once

#include <array>
#include "CollisionBox.hpp"
#include "CollisionTestConstants.hpp"
#include "Position.hpp"

/**
 * @brief The two square boxes of a body: 30 units left and 30 right of its
 * Position, from x -35 to -25 and from 25 to 35. A square on that Position,
 * from -5 to 5, touches neither, though it lies inside the rectangle that
 * encloses them; a square on RIGHT_PART_CENTER_X lies on the right one.
 */
constexpr float RIGHT_PART_CENTER_X = 30.0F;
constexpr std::array PARTS_APART{
    rtype::game::CollisionBox{.width = SQUARE_SIDE,
                              .height = SQUARE_SIDE,
                              .offsetX = -RIGHT_PART_CENTER_X},
    rtype::game::CollisionBox{.width = SQUARE_SIDE,
                              .height = SQUARE_SIDE,
                              .offsetX = RIGHT_PART_CENTER_X}};

/**
 * @brief The two square boxes of a body that overlap each other and both
 * cover its Position: 2 units left and 2 right of it, from x -7 to 3 and from
 * -3 to 7.
 */
constexpr float CLOSE_PART_OFFSET_X = 2.0F;
constexpr std::array PARTS_ON_ONE_SPOT{
    rtype::game::CollisionBox{.width = SQUARE_SIDE,
                              .height = SQUARE_SIDE,
                              .offsetX = -CLOSE_PART_OFFSET_X},
    rtype::game::CollisionBox{.width = SQUARE_SIDE,
                              .height = SQUARE_SIDE,
                              .offsetX = CLOSE_PART_OFFSET_X}};

/**
 * @brief The ship of the shipped sheet laid with its mask (32 by 16 pixels) on
 * cell 0, 0: 64 / 4 - 16 = 0 and 32 / 4 - 8 = 0. Its box covers 0 to 128 and
 * 0 to 64 units.
 */
constexpr rtype::game::Position SHIP_ON_CELL_ZERO{.x = 64.0F, .y = 32.0F};

/**
 * @brief The missile of a Bydo of the shipped sheet at the top-left corner of
 * that ship: its box (32 by 32, from -16 to 16) overlaps the box of the ship,
 * and its mask (8 by 8, on cells -4 to 3) lies over rows 0 to 3 and columns 0
 * to 3 of the ship, which masks/ship.txt leaves clear.
 */
constexpr rtype::game::Position MISSILE_OVER_THE_CLEAR_CORNER{.x = 0.0F,
                                                              .y = 0.0F};

/** @brief The missile of a Bydo on the middle of that ship, cells 12 to 19
 * and 4 to 11, where the hull is drawn. */
constexpr rtype::game::Position MISSILE_ON_THE_HULL{.x = 64.0F, .y = 32.0F};
