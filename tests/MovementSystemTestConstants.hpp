#pragma once

#include <chrono>
#include <cstdint>
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

}  // namespace rtype::game::testing
