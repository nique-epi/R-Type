#pragma once

#include <cstddef>
#include "Position.hpp"
#include "ShotPattern.hpp"

namespace rtype::game::testing {

/** @brief The start of the single projectile of a shot, in front of a gun. */
constexpr Position SHOT_START{.x = 400.0F, .y = 300.0F};

/** @brief A fan of three projectiles over 24 degrees, laid 16 units ahead:
 * they head -12, 0 and 12 degrees from the way of the gun, so the middle one
 * lies 16 units ahead and the side ones 16 cos 12° = 15.650 units ahead and
 * 16 sin 12° = 3.327 units above or below. */
constexpr ShotPattern FAN_OF_THREE{
    .count = 3, .spreadDegrees = 24.0F, .stepY = 0.0F, .distance = 16.0F};
constexpr std::size_t FIRST_OF_THE_FAN = 0;
constexpr std::size_t MIDDLE_OF_THE_FAN = 1;
constexpr std::size_t LAST_OF_THE_FAN = 2;
constexpr float FAN_DISTANCE_AHEAD = 16.0F;
constexpr float FAN_SIDE_AHEAD = 15.650F;
constexpr float FAN_SIDE_ASIDE = 3.327F;

/** @brief Two rows 24 units apart, laid on the start: 12 units above it and
 * 12 below. */
constexpr ShotPattern TWO_ROWS{
    .count = 2, .spreadDegrees = 0.0F, .stepY = 24.0F, .distance = 0.0F};
constexpr std::size_t UPPER_ROW = 0;
constexpr std::size_t LOWER_ROW = 1;
constexpr float HALF_THE_ROWS = 12.0F;

/** @brief One projectile laid 16 units ahead of the start. */
constexpr ShotPattern ONE_AHEAD{
    .count = 1, .spreadDegrees = 0.0F, .stepY = 0.0F, .distance = 16.0F};
constexpr std::size_t ONLY_SHOT = 0;

/** @brief How far a computed position may be from the expected one. */
constexpr float SHOT_TOLERANCE_UNITS = 0.01F;

}  // namespace rtype::game::testing
