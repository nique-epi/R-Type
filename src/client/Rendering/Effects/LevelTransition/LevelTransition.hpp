#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns How far along x the ships of the players are drawn from their
 * place, in logical units, with @p left of a transition of the level: 0 before
 * its last parts and once it ended; while the ships fly out
 * (game::LEVEL_TRANSITION_FLY_OUT), from 0 to LEVEL_TRANSITION_OUT_DISTANCE,
 * slowly then fast; while they stay away, LEVEL_TRANSITION_OUT_DISTANCE; while
 * they fly in (game::LEVEL_TRANSITION_FLY_IN), from
 * -LEVEL_TRANSITION_IN_DISTANCE to 0, fast then slowly. The server puts them
 * back at their start as they start flying in.
 */
[[nodiscard]] float shipShiftAt(engine::Duration left);

/**
 * @returns How many times their own speed the stars move with @p left of a
 * transition of the level: 1 before its last parts and once it ended; from 1
 * to WARP_TOP_SPEED_FACTOR while the ships fly out, at it while they stay
 * away, back to 1 while they fly in, fast then slowly, as after MISSION START.
 */
[[nodiscard]] float transitionWarpAt(engine::Duration left);

}  // namespace rtype::client
