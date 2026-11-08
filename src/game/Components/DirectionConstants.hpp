#pragma once

#include <numbers>

namespace rtype::game {

/** @brief The share of a Direction that asks for the full speed of a body on
 * one axis; a larger share counts as this one. */
constexpr float FULL_STEERING = 1.0F;

/** @brief The radians in a degree: a sheet gives its angles in degrees. */
constexpr float RADIANS_PER_DEGREE = std::numbers::pi_v<float> / 180.0F;

}  // namespace rtype::game
