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
 * Provisional. It stays under BYDO_SPAWN_VERTICAL_MARGIN so a Bydo on a wave
 * never leaves the height of the playfield.
 */
constexpr float WAVE_AMPLITUDE = 48.0F;

/**
 * @brief How long a Wave takes to go up, down and back to its row, in seconds.
 */
constexpr float WAVE_PERIOD_SECONDS = 2.0F;

static_assert(WAVE_AMPLITUDE < BYDO_SPAWN_VERTICAL_MARGIN);

}  // namespace rtype::game
