#pragma once

#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief The time between two updates of the bots: one simulation tick, the
 * pace of the frames of a client, so the PlayerInputPacer of each bot sends
 * PLAYER_INPUTS_PER_SECOND inputs per second.
 */
constexpr engine::Duration BOT_FRAME_DURATION =
    engine::SIMULATION_TICK_DURATION;

}  // namespace rtype::tools
