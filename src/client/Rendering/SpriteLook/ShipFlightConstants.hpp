#pragma once

#include <array>
#include <cstddef>

namespace rtype::client {

/**
 * @brief The flame of a ship follows the thrust its pilot asks for at
 * FLAME_RESPONSE_RATE per second: full while it pushes forward or dashes,
 * FLAME_MANEUVER_SHARE while it only moves up or down, out otherwise. It
 * lights before the drift has gathered speed and goes out as the glide starts.
 */
constexpr float FLAME_RESPONSE_RATE = 14.0F;
constexpr float FLAME_MANEUVER_SHARE = 0.35F;

/**
 * @brief The ages, in frames, of the three afterimages drawn while a ship
 * rolls or dashes, oldest first, and the frames of the trail it keeps for
 * them: one more than the oldest age.
 */
constexpr std::array<std::size_t, 3> AFTERIMAGE_AGES{12, 8, 4};
constexpr std::size_t AFTERIMAGE_TRAIL_LENGTH = AFTERIMAGE_AGES.front() + 1;

/**
 * @brief A roll draws one whole turn of the ship around its length (FULL_TURN
 * of BackgroundConstants.hpp); the ship keeps at least ROLL_THINNEST_ROWS
 * rows of its frame when seen edge on.
 */
constexpr int ROLL_THINNEST_ROWS = 2;

}  // namespace rtype::client
