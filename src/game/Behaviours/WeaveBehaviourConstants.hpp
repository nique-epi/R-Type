#pragma once

#include <numbers>
#include "SpawningConstants.hpp"

namespace rtype::game {

/**
 * @brief How far a weave goes above and below its row, in logical units.
 *
 * Provisional. It stays within BYDO_PATTERN_ROOM, the room every row keeps
 * above and below it, so a body on a weave stays in the area of play.
 */
constexpr float WAVE_AMPLITUDE = 48.0F;

/**
 * @brief How long a weave takes to go down, up and back to its row, in
 * seconds.
 */
constexpr float WAVE_PERIOD_SECONDS = 2.0F;

/** @brief One whole turn of the sine a weave follows, in radians. */
constexpr float FULL_TURN_RADIANS = 2.0F * std::numbers::pi_v<float>;

static_assert(WAVE_AMPLITUDE <= BYDO_PATTERN_ROOM,
              "a body on a weave stays within the room its row keeps");

}  // namespace rtype::game
