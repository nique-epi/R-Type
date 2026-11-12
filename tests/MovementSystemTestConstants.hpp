#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "CollisionBox.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief Where the moving entities of the movement tests start, in logical
 * units. */
constexpr float START_X = 100.0F;
constexpr float START_Y = 200.0F;

/** @brief How fast they move, in logical units per second. */
constexpr float SPEED_X = 120.0F;
constexpr float SPEED_Y = -60.0F;

constexpr engine::Duration HALF_SECOND = std::chrono::milliseconds(500);

/** @brief Where an entity from (START_X, START_Y) at (SPEED_X, SPEED_Y)
 * arrives after HALF_SECOND. */
constexpr float HALF_SECOND_ARRIVAL_X = 160.0F;
constexpr float HALF_SECOND_ARRIVAL_Y = 170.0F;

constexpr std::int64_t NANOSECONDS_PER_SECOND = 1'000'000'000;

/** @brief How far a position added up tick by tick may be from the exact one,
 * in logical units. */
constexpr float POSITION_TOLERANCE_UNITS = 1e-4F;

/** @brief The top speed of a steady body of the movement tests, in logical
 * units per second. */
constexpr float STEADY_TOP_SPEED = 240.0F;

/** @brief A share of half the top speed, and one beyond the full speed. */
constexpr float HALF_STEERING = 0.5F;
constexpr float TWICE_THE_FULL_STEERING = 2.0F;

/** @brief Where a steady body from (START_X, START_Y) at 240 units per second
 * arrives after HALF_SECOND: full speed left is 120 units, half speed down 60.
 */
constexpr float STEADY_ARRIVAL_X = -20.0F;
constexpr float STEADY_ARRIVAL_Y = 260.0F;

/** @brief A ship that starts at x 300 and holds right for a second, which
 * takes it far past x 404, and a wall 64 by 256 centered at x 500 on its row:
 * a ship touches its left side with its center at x 404 (468 - 64). */
constexpr float SHIP_START_X = 300.0F;
constexpr float WALL_ROW_Y = 400.0F;
constexpr float WALL_X = 500.0F;
constexpr CollisionBox WALL_BOX{.width = 64.0F, .height = 256.0F};
constexpr float AGAINST_THE_WALL_X = 404.0F;

}  // namespace rtype::game::testing
