#pragma once

#include <array>
#include "CollisionBox.hpp"
#include "CollisionTestConstants.hpp"

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
