#pragma once

#include "Bounds.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/**
 * @brief A wall 64 by 256 centered at (500, 400): its sides at x 468 and 532,
 * its top at y 272. A ship of 128 by 64 touches its left side with its center
 * at x 404 (468 - 64), its top with its center at y 240 (272 - 32).
 */
constexpr engine::Bounds WALL{
    .centerX = 500.0F, .centerY = 400.0F, .width = 64.0F, .height = 256.0F};
constexpr float WALL_ROW_Y = 400.0F;
constexpr float WALL_MIDDLE_X = 500.0F;
constexpr float TOUCHING_THE_LEFT_SIDE_X = 404.0F;
constexpr float TOUCHING_THE_TOP_Y = 240.0F;

/** @brief A ship that flies right into the wall: from x 396, clear of it, to
 * x 410, 6 units into it, at full speed. */
constexpr Position CLEAR_ON_THE_LEFT{.x = 396.0F, .y = WALL_ROW_Y};
constexpr Position INTO_THE_LEFT_SIDE{.x = 410.0F, .y = WALL_ROW_Y};
constexpr float FULL_SPEED_RIGHT = 480.0F;
constexpr float SOME_SPEED_DOWN = 100.0F;

/** @brief A ship that comes down onto the wall: from y 230, clear of it, to
 * y 250, 10 units into it. */
constexpr Position CLEAR_ABOVE{.x = WALL_MIDDLE_X, .y = 230.0F};
constexpr Position INTO_THE_TOP{.x = WALL_MIDDLE_X, .y = 250.0F};
constexpr float SOME_SPEED_UP_THE_SCREEN = 300.0F;

/** @brief The wall moved 2 units left by the scroll, onto a ship that touched
 * its left side: 2 units deep across, 160 down, so the ship goes to x 402. */
constexpr engine::Bounds WALL_MOVED_ONTO_THE_SHIP{
    .centerX = 498.0F, .centerY = 400.0F, .width = 64.0F, .height = 256.0F};
constexpr Position TOUCHING_THE_LEFT_SIDE{.x = TOUCHING_THE_LEFT_SIDE_X,
                                          .y = WALL_ROW_Y};
constexpr float PUSHED_BY_THE_WALL_X = 402.0F;

}  // namespace rtype::game::testing
