#pragma once

namespace rtype::client {

/** @brief How far to the right the ships of the players fly out during a
 * transition of the level, in logical units: past the right edge of the
 * playfield (1920) from any column a ship may hold. */
constexpr float LEVEL_TRANSITION_OUT_DISTANCE = 2176.0F;

/** @brief How far to the left of their start the ships come back from, in
 * logical units: their start, 192 units from the left edge, then the length
 * of a ship and a margin, so each enters from beyond the edge. */
constexpr float LEVEL_TRANSITION_IN_DISTANCE = 448.0F;

}  // namespace rtype::client
