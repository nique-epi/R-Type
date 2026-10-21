#pragma once

#include "SpawningConstants.hpp"

namespace rtype::game {

/**
 * @brief How fast a Bydo moves to the left, in logical units per second.
 *
 * Provisional: crossing the 1920 units of the playfield takes 8 seconds.
 */
constexpr float BYDO_SPEED = 240.0F;

/**
 * @brief How far a Wave goes above and below its row, in logical units.
 *
 * Provisional. It stays within BYDO_PATTERN_ROOM, the room every row keeps
 * above and below it, so a Bydo on a wave stays in the area of play.
 */
constexpr float WAVE_AMPLITUDE = 48.0F;

/**
 * @brief How long a Wave takes to go up, down and back to its row, in seconds.
 */
constexpr float WAVE_PERIOD_SECONDS = 2.0F;

static_assert(WAVE_AMPLITUDE <= BYDO_PATTERN_ROOM,
              "a Bydo on a wave stays within the room its row keeps");

}  // namespace rtype::game
